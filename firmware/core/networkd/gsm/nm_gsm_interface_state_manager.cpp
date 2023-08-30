#include "networkd/gsm/nm_gsm_interface_state_manager.h"
#include "networkd/interfaces/nm_gsm_interface.h"
#include "networkd/resources/nm_resources.h"

#include "log_utils/ezlo_log.h"
#include "tiny/include/tiny/tiny_consolecolors.h"
#include "service_interfaces/task_loop.h"
#include "service_interfaces/task_loop_timer.h"

#include <cassert>
#include <chrono>
#include <utility>
#include <tuple>

namespace HubPlatform {
namespace Network {
namespace Mobile {

namespace {
const auto initTimeout = std::chrono::minutes( 1 );
const auto isRegisteredFailedTimeout = std::chrono::minutes( 3 );
}  // namespace

InterfaceStateManager::InterfaceStateManager( Services::TaskLoop& taskLoop,
                                              GSMInterface& interface,
                                              const NetworkConfigFileManager::MobileConfiguration& configuration )
: m_taskLoop( taskLoop )
, m_interface( interface )
, m_modem( *m_interface.m_gsmModem )
, m_apnSettingsProvider( m_interface.m_apnSettingsProvider )
, m_modemInfoPollingTimer( m_interface.m_modemInfoPollingTimer )
, m_interfaceName( m_interface.getInterfaceName() )
, m_networkManager( m_modem, configuration, m_interfaceName )
, m_connectionManager( m_interface, *this, m_taskLoop, configuration ) {}

void InterfaceStateManager::start( const std::weak_ptr< GSMInterface >& weakSelf ) {
    m_weakSelf = weakSelf;
    m_connectionManager.start( m_weakSelf );
    scheduleRun( initTimeout );
}

void InterfaceStateManager::scheduleRun( const std::chrono::seconds& relativeStartTime ) {
    auto weakSelf = m_weakSelf;
    m_taskLoop
        .createPostponedTask( [this, weakSelf]( Services::PostponedTask& ) {
            auto interface = weakSelf.lock();
            if( !interface ) {
                LOGGER_WARN_RAW( cc::warn( "Mobile interface already removed before scheduleRun" ) );
                return;
            }

            try {
                run();
            }
            catch( const std::exception& e ) {
                LOGGER_ERROR_RAW( cc::error( "Failed to run state machine, interface: " ) +
                                  cc::note( m_interfaceName ) + cc::error( ", " ) + cc::note( e.what() ) );

                m_state = State::MODEM_HUNGUP;
                handleModemHungUp();
            }
            catch( ... ) {
                LOGGER_WARN_RAW( cc::warn( "Unknown exception was caught during running mobile state manager" ) );
            }
        } )
        ->start( relativeStartTime, false );
}

void InterfaceStateManager::onConnectionInterrupted( bool probablyFalsePositive ) {
    m_connectionManager.onConnectionInterrupted( probablyFalsePositive );
}

void InterfaceStateManager::onConnectionRestored() {
    m_connectionManager.onConnectionRestored();
}

void InterfaceStateManager::run() {
    switch( m_state ) {
    case State::MODEM_UP:
        handleModemUp();
        break;
    case State::SIM_PRESENT:
        handleSIMPresent();
        break;
    case State::SIM_NOT_PRESENT:
        handleSIMNotPresent();
        break;
    case State::PIN_REQUIRED:
        handlePINRequired();
        break;
    case State::CHECK_REGISTRATION:
        handleCheckRegistration();
        break;
    case State::MODEM_HUNGUP:
        handleModemHungUp();
        break;
    case State::MODEM_DOWN:
        handleModemDown();
        break;
    default:
        assert( false );
    }
}

void InterfaceStateManager::handleModemUp() {
    m_apnSettingsProvider.init();

    if( !m_modem.isSIMInserted() ) {
        m_state = State::SIM_NOT_PRESENT;
        handleSIMNotPresent();

        return;
    }

    m_state = State::SIM_PRESENT;
    handleSIMPresent();
}

void InterfaceStateManager::handleSIMPresent() {
    if( m_modem.isPINCodeRequired() && !m_interface.enterPinCodeAutomatically() ) {
        m_state = State::PIN_REQUIRED;
        handlePINRequired();

        return;
    }

    LOGGER_DEBUG_RAW( cc::debug( "SIM Lock is turned " ) + cc::note( ( m_modem.isLocked() ? "on" : "off" ) ) +
                      cc::debug( ", interface: " ) + cc::note( m_interfaceName ) );

    m_state = State::CHECK_REGISTRATION;
    handleCheckRegistration();
}

void InterfaceStateManager::handleCheckRegistration() {
    m_modemInfoPollingTimer.stop();

    auto result = isRegistered();
    if( !result.first ) {
        if( m_isRegistered ) {
            m_isRegistered = false;
            LOGGER_WARN_RAW( cc::warn( "No registration on a mobile interface, interface: " ) +
                             cc::note( m_interfaceName ) );

            m_interface.onRegisrationInterrupted();
        }

        scheduleRun( result.second );

        return;
    }
    else {
        m_isRegistered = true;
        m_interface.onRegisrationEstablished();

        return;
    }
}

void InterfaceStateManager::handleSIMNotPresent() {
    LOGGER_WARN_RAW( cc::warn( "SIM card isn't present, interface: " ) + cc::note( m_interfaceName ) );
    m_interface.notifyError( Errors::MODEM_SIM_NO_PRESENT );
}

void InterfaceStateManager::handlePINRequired() {
    LOGGER_WARN_RAW( cc::warn( "SIM PIN code is required, interface: " ) + cc::note( m_interfaceName ) );
    m_interface.notifyError( Errors::MODEM_SIM_PIN_CODE_IS_REQUIRED );
}

void InterfaceStateManager::handleModemHungUp() {
    try {
        LOGGER_WARN_RAW( cc::warn( "Mobile modem hang up, interface: " ) + cc::note( m_interfaceName ) );
        m_modem.reboot();

        m_state = State::MODEM_DOWN;
    }
    catch( std::exception& e ) {
        LOGGER_ERROR_RAW( cc::error( "Failed to handle modem hung up on interface: " ) + cc::note( m_interfaceName ) +
                          cc::error( ", " ) + cc::note( e.what() ) );
        m_state = State::MODEM_HUNGUP;
    }

    m_interface.notifyError( Errors::MODEM_HUNG );
}

void InterfaceStateManager::handleModemDown() {
    LOGGER_DEBUG_RAW( cc::debug( "Mobile modem is down, interface: " ) + cc::note( m_interfaceName ) );
}

std::pair< bool, std::chrono::seconds > InterfaceStateManager::isRegistered() {
    std::chrono::seconds timeout( isRegisteredFailedTimeout );
    bool isRegistered = false;

    try {
        std::tie( isRegistered, timeout ) = m_networkManager.isRegistered();
    }
    catch( const std::exception& e ) {
        LOGGER_ERROR_RAW( cc::error( "Failed to check is interface registered, interface: " ) +
                          cc::note( m_interfaceName ) + cc::error( ", reason " ) + cc::note( e.what() ) );
    }
    catch( ... ) {
        LOGGER_ERROR_RAW( cc::error( "Failed to check is interface registered, interface: " ) +
                          cc::note( m_interfaceName ) );
    }

    return { isRegistered, timeout };
}

void InterfaceStateManager::reRegister( bool cleanFPLMN ) {
    m_modem.reRegister( cleanFPLMN );
}

void InterfaceStateManager::rebootModem() {
    m_modem.reboot();
}

}  // namespace Mobile
}  // namespace Network
}  // namespace HubPlatform
