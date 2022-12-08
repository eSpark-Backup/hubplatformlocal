#pragma once

#include <cstdint>

namespace znet {

class ProvisioningListEntry;

/**
 * Handles updates of provisioning list entries initiated by ProvisioningListController.
 */
class IProvisioningListObserver {
public:
    virtual ~IProvisioningListObserver() = default;

    virtual void onProvisioningListEntryAdded( const ProvisioningListEntry& ) = 0;
    virtual void onProvisioningListEntryUpdated( const ProvisioningListEntry&, const ProvisioningListEntry& ) = 0;
    virtual void onProvisioningListEntryRemoved( uint32_t provisioningId ) = 0;
};

}  // namespace znet
