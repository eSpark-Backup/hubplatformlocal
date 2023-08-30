#pragma once

#include <znet/iznet_provisioning_list_controller.hpp>

namespace znet {
namespace api {

/**
 * Interface adapter for Provisioning List entry data source used by ProvisioningListQuery.
 */
class IProvisioningListQueryDataSource {
public:
    using iterate_function_t = std::function<bool( const ::znet::ProvisioningListEntry& )>;

    virtual ~IProvisioningListQueryDataSource() = default;

    /**
     * Iterates through the data source and calls a given @param iterateFunction for each entry..
     *
     * @param iterateFunction A function subsequently called for each entry.
     *                        Must return true to stop iteration with SUCCESS or false to continue until
     *                        ENTRY_NOT_FOUND.
     * @return OperationStatus received from the data source.
     */
    virtual ::znet::api::IProvisioningListController::OperationStatus iterate(
        const iterate_function_t& iterateFunction ) const = 0;

    virtual ::znet::api::IProvisioningListController::OperationStatus update( const ProvisioningListEntry& ) = 0;

    virtual ::znet::api::IProvisioningListController::OperationStatus remove( uint32_t id ) = 0;
};

}  // namespace api
}  // namespace znet
