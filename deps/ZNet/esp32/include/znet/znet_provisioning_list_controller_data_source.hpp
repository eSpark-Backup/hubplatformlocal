#pragma once

#include <znet/iznet_provisioning_list_query_data_source.hpp>

namespace znet {
namespace api {

/**
 * Adapts interface of IProvisioningListController to IProvisioningListQueryDataSource.
 */
class ProvisioningListControllerDataSource : public IProvisioningListQueryDataSource {
public:
    explicit ProvisioningListControllerDataSource(::znet::api::IProvisioningListController& controller )
    : m_controller( controller ) {}

    ::znet::api::IProvisioningListController::OperationStatus iterate(
        const iterate_function_t& iterateFunction ) const override {
        return m_controller.findProvisioningListEntry( iterateFunction );
    }

    ::znet::api::IProvisioningListController::OperationStatus update( const ProvisioningListEntry& entry ) override {
        return m_controller.setProvisioningListEntry( entry.m_provisioningId, entry.m_fields );
    }

    ::znet::api::IProvisioningListController::OperationStatus remove( uint32_t id ) override {
        return m_controller.removeProvisioningListEntry( id );
    }

private:
    ::znet::api::IProvisioningListController& m_controller;
};

}  // namespace api
}  // namespace znet
