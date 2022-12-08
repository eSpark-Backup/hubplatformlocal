#include "networkd/scan/nm_wifi_passive_scan_task_base.h"
#include "networkd/scan/nm_scan_utils.h"

#include "log_utils/ezlo_log.h"
#include "tiny/include/tiny/tiny_consolecolors.h"

#include <thread>

namespace HubPlatform {
namespace Network {

std::mutex WifiPassiveScanTaskBase::m_mutex;

WifiPassiveScanTaskBase::WifiPassiveScanTaskBase( std::string interfaceName,
                                                  WifiScanResultListener& scanResultListener )
: WifiPassiveScanTask( std::move( interfaceName ), scanResultListener )
, m_scanStartedDuration( std::chrono::seconds( 0 ) ) {}

WifiPassiveScanTaskBase::~WifiPassiveScanTaskBase() {
    stopWorkerAndWait();
}

void WifiPassiveScanTaskBase::start() {
    if( m_isRunning ) {
        // Reset elapsed time of scanner's timeout.
        m_scanStartedDuration = std::chrono::steady_clock::now().time_since_epoch();
        return;
    }
    else {
        if( m_worker.joinable() ) {
            m_worker.join();
        }
    }

    m_worker = std::thread( [this] {
        LOGGER_DEBUG_RAW( cc::debug( "wifi passive scan started on interface " ) + cc::note( m_interfaceName ) );
        m_stopped = false;
        m_isRunning = true;
        std::unique_ptr< decltype( m_isRunning ), void ( * )( decltype( m_isRunning )* ) > guard(
            &m_isRunning, []( decltype( m_isRunning )* val ) { *val = false; } );

        m_scanStartedDuration = std::chrono::steady_clock::now().time_since_epoch();

        beforeScan();
        while( !m_stopped ) {
            doScan();
            sendBroadcast();
            if( m_stopped )
                break;
            std::this_thread::sleep_for( getScanInterval() );

            auto scanDuration = std::chrono::steady_clock::now().time_since_epoch() - m_scanStartedDuration.load();
            if( scanDuration >= getScanTimeout() ) {
                LOGGER_DEBUG_RAW( cc::debug( "wifi passive scan timeout on interface " ) + cc::note( m_interfaceName ) +
                                  cc::debug( ", stopping" ) );
                stop();
            }
        }
        LOGGER_DEBUG_RAW( cc::debug( "wifi passive scan stopped on interface " ) + cc::note( m_interfaceName ) );
        onStopped();
    } );
}

void WifiPassiveScanTaskBase::stop() {
    m_stopped = true;
}

std::chrono::milliseconds WifiPassiveScanTaskBase::getScanInterval() const {
    return std::chrono::seconds( 8 );
}

std::chrono::seconds WifiPassiveScanTaskBase::getScanTimeout() const {
    return std::chrono::seconds( 300 );
}

void WifiPassiveScanTaskBase::beforeScan() {
    m_scanResultListener.onScanStarted( m_interfaceName );
}

void WifiPassiveScanTaskBase::doScan() {
    std::unique_lock< std::mutex > lock( m_mutex );

    try {
        auto newlyScanned = scanWifiToMap( m_interfaceName );
        // Add newly appeared networks and update already added (signal level may change).
        for( const auto& item : newlyScanned ) {
            m_scanned[ item.first ] = item.second;
        }
    }
    catch( const std::exception& ex ) {
        lock.unlock();
        m_scanResultListener.onScanFailed( m_interfaceName, ex.what() );
    }
}

void WifiPassiveScanTaskBase::sendBroadcast() {
    if( !m_stopped && !m_scanned.empty() ) {
        JsonArray networks;
        for( const auto& it : m_scanned ) {
            networks.push_back( networkToJson( it.second ) );
        }
        m_scanResultListener.onScanProcess( m_interfaceName, std::move( networks ) );
    }

    // Clear scanned networks.
    m_scanned.clear();
}

void WifiPassiveScanTaskBase::onStopped() {
    m_scanned.clear();
    m_scanResultListener.onScanFinished( m_interfaceName );
}

void WifiPassiveScanTaskBase::stopWorkerAndWait() {
    if( m_worker.joinable() ) {
        m_stopped = true;
        m_worker.join();
    }
}

Json WifiPassiveScanTaskBase::manualScan( const std::string& interfaceName, const std::string& ssid ) {
    std::lock_guard< std::mutex > lock( m_mutex );

    Json result;
    try {
        JsonArray networks;
        auto scanResult = scanWifiToMap( interfaceName, ssid );
        for( const auto& item : scanResult ) {
            networks.push_back( networkToJson( item.second ) );
        }
        result[ "networks" ] = std::move( networks );
    }
    catch( const std::exception& ex ) {
    }

    return result;
}

}  // namespace Network
}  // namespace HubPlatform
