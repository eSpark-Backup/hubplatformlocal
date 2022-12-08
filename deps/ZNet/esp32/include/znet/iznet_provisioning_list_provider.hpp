#pragma once

#include <functional>

#include <znet/znet_provisioning_list_entry.hpp>

namespace znet {

/**
 * Provides storage for provisioning list entries to ProvisioningListController.
 * The interface covers firmware-specific part of the implementation.
 */
class IProvisioningListProvider {
public:
    using ProcessProvisioningCallback = std::function<bool( const ProvisioningListEntry& )>;

    virtual ~IProvisioningListProvider() = default;

    virtual bool storeProvisioningListEntry( const ProvisioningListEntry& ) = 0;
    virtual bool removeProvisioningListEntryByID( uint32_t ) = 0;
    virtual bool findProvisioningListEntryByID( uint32_t, ProvisioningListEntry& ) const = 0;
    virtual bool findProvisioningListEntryByDSK( const DSK&, ProvisioningListEntry& ) const = 0;
    virtual size_t getProvisioningListEntryCount() const = 0;

    /**
     * Subsequently calls a given callback for each entry in the Provisioning List until the callback returns true.
     *
     * @return true if enumeration succeeded, false in case of an error
     */
    virtual bool loadProvisioningList( const ProcessProvisioningCallback& ) const = 0;
};

}  // namespace znet
