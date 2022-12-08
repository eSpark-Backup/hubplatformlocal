#pragma once

#include "opkg_utils/opkg_manager.h"

namespace HubPlatform {
namespace UpdateManager {

class OpkgSession final {
public:
    OpkgSession( Opkg::OpkgManager& opkgManager );
    ~OpkgSession();

    bool updateFeeds( const std::string& source, int snapshot );
    void commit();

    bool verifyAvailablePackageVersion( Opkg::PackageSummary& packageSummary );

    bool updateAvailablePackages();

    const Opkg::IFeedsUpdater& getFeedsUpdater() const;

private:
    Opkg::IFeedsUpdater& takeFeedsUpdater();
    Opkg::IPackagesManager& takePackagesManager();

private:
    Opkg::OpkgManager& m_opkgManager;
    mutable std::unique_ptr< Opkg::IFeedsUpdater > m_feedsUpdater;
    mutable std::unique_ptr< Opkg::IPackagesManager > m_packagesManager;
    bool m_feedsUpdated;
    bool m_opkgListsUpdated;
};

}  // namespace UpdateManager
}  // namespace HubPlatform
