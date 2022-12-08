#include "update_manager/um_opkg_session.h"

#include "system_utils/filesystem.h"
#include "log_utils/ezlo_log.h"

#include <chrono>
#include <thread>

namespace HubPlatform {
namespace UpdateManager {

namespace {
constexpr const int RetriesCount = 2;
constexpr const std::chrono::seconds RetriesDelay( 10 );
constexpr const char OpkgListsPath[] = "/tmp/opkg-lists";
}  // namespace

OpkgSession::OpkgSession( Opkg::OpkgManager& opkgManager )
: m_opkgManager( opkgManager )
, m_feedsUpdated( false )
, m_opkgListsUpdated( false ) {}

OpkgSession::~OpkgSession() {
    if( m_feedsUpdated ) {
        takeFeedsUpdater().resetToCurrent();
    }
}

bool OpkgSession::updateFeeds( const std::string& source, int snapshot ) {
    m_feedsUpdated = true;
    return takeFeedsUpdater().update( source, snapshot );
}

void OpkgSession::commit() {
    m_feedsUpdated = false;
    takeFeedsUpdater().resetCurrent();
}

bool OpkgSession::updateAvailablePackages() {
    if( m_opkgListsUpdated ) {
        // Assume lists were not changed during this opkg session
        return true;
    }

    std::string output;
    int result = 0;
    Log::info( "Updating available packages list" );
    for( int i = 0; i < RetriesCount + 1; ++i ) {
        if( i != 0 ) {
            Log::warn( "Retry #" + std::to_string( i ) + " in " + std::to_string( RetriesDelay.count() ) + " seconds" );
            std::this_thread::sleep_for( RetriesDelay );
        }
        std::tie( output, result ) = takePackagesManager().updateAvailable();
        Log::debug( output );

        if( result == 0 )
            break;
    }

    if( result != 0 ) {
        Log::error( "Failed to update available packages list" );
        return false;
    }

    m_opkgListsUpdated = true;
    return true;
}

bool OpkgSession::verifyAvailablePackageVersion( Opkg::PackageSummary& packageSummary ) {
    if( !updateAvailablePackages() ) {
        Log::warn( "Update available packages failed" );
        return false;
    }

    if( packageSummary.version.empty() ) {
        auto isProperPackage = [&]( const Opkg::PackageSummary& package ) {
            return package.name == packageSummary.name;
        };

        auto packages = takePackagesManager().getAvailablePackages();
        auto it = std::find_if( packages.begin(), packages.end(), isProperPackage );
        if( it == packages.end() ) {
            Log::info( "Package " + packageSummary.name + " is not available" );
            return false;
        }

        packageSummary.version = it->version;
    }
    else {
        auto isProperPackage = [&]( const Opkg::PackageSummary& package ) {
            return ( package.name == packageSummary.name ) && ( package.version == packageSummary.version );
        };

        auto packages = takePackagesManager().getAvailablePackages();
        auto it = std::find_if( packages.begin(), packages.end(), isProperPackage );
        if( it == packages.end() ) {
            Log::info( "Requested " + packageSummary.name + " version is not available" );
            return false;
        }
    }

    return true;
}

const Opkg::IFeedsUpdater& OpkgSession::getFeedsUpdater() const {
    if( !m_feedsUpdater ) {
        m_feedsUpdater = m_opkgManager.createFeedsUpdater();
    }
    return *m_feedsUpdater;
}

Opkg::IFeedsUpdater& OpkgSession::takeFeedsUpdater() {
    if( !m_feedsUpdater ) {
        m_feedsUpdater = m_opkgManager.createFeedsUpdater();
    }
    return *m_feedsUpdater;
}

Opkg::IPackagesManager& OpkgSession::takePackagesManager() {
    if( !m_packagesManager ) {
        m_packagesManager = m_opkgManager.createPackagesManager();
    }
    return *m_packagesManager;
}

}  // namespace UpdateManager
}  // namespace HubPlatform
