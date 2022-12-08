#pragma once

#include "znet/iznet_provisioning_list_controller.hpp"

namespace znet {

class IProvisioningListProvider;
class IProvisioningListObserver;

namespace api {

class IProvisioningListQueryDataSource;

/**
 * Implementation of a Provisioning List controller.
 * Depends on firmware-specific parts of the implementation:
 *   - static storage is represented by IProvisioningListProvider.
 *   - event handler for entry updates is represented by IProvisioningListObserver.
 */
class ProvisioningListController : public IProvisioningListController {
public:
    ProvisioningListController( IProvisioningListQueryDataSource&,
                                IProvisioningListProvider&,
                                IProvisioningListObserver& );

    /**
     * On success causes IProvisioningListObserver::onProvisioningListEntryAdded for a new entry or
     * IProvisioningListObserver::onProvisioningListEntryUpdated for update of an existing entry.
     */
    OperationStatus addProvisioningListEntry( const DSK& dsk, const ProvisioningListEntryFields& newFields ) override;

    /**
     * On success causes IProvisioningListObserver::onProvisioningListEntryUpdated.
     */
    OperationStatus setProvisioningListEntry( uint32_t provisioning_id,
                                              const ProvisioningListEntryFields& newFields ) override;

    /**
     * On success causes IProvisioningListObserver::onProvisioningListEntryRemoved.
     */
    OperationStatus removeProvisioningListEntry( uint32_t provisioningId ) override;

    OperationStatus findProvisioningListEntry( const ProcessProvisioningCallback& callback ) const override;

private:
    static uint32_t generateProvisioningID( const DSK& dsk );
    static ProvisioningListEntryFields setDefaults( const ProvisioningListEntryFields& );

    template <typename FieldType, typename ValueType>
    static void setFieldDefaultValue( utils::optional<FieldType>&, const ValueType& );

    void mergeWithClassicInclusionData( const DSK& dsk, ProvisioningListEntryFields& newEntry );

    bool isCorrectAddingMode( const std::vector<ProvisioningListEntryFields::AddingMode>& supportedModes,
                              const ProvisioningListEntryFields::AddingMode& mode ) const;

private:
    IProvisioningListQueryDataSource& m_classisInclusionSecurityDataSource;
    IProvisioningListProvider& m_provider;
    IProvisioningListObserver& m_observer;
};

}  // namespace api
}  // namespace znet
