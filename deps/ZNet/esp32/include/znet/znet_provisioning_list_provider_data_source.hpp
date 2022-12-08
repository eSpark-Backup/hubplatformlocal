#pragma once

#include <znet/iznet_provisioning_list_query_data_source.hpp>
#include <znet/iznet_provisioning_list_provider.hpp>

namespace znet {
namespace api {

/**
 * Adapts interface of IProvisioningListController to IProvisioningListQueryDataSource.
 */
class ProvisioningListProviderDataSource : public IProvisioningListQueryDataSource {
public:
    explicit ProvisioningListProviderDataSource(::znet::IProvisioningListProvider& provider );

    ::znet::api::IProvisioningListController::OperationStatus iterate(
        const iterate_function_t& iterateFunction ) const override;

    ::znet::api::IProvisioningListController::OperationStatus update( const ProvisioningListEntry& entry ) override;

    ::znet::api::IProvisioningListController::OperationStatus remove( uint32_t id ) override;

private:
    ::znet::IProvisioningListProvider& m_provider;
};

}  // namespace api
}  // namespace znet
