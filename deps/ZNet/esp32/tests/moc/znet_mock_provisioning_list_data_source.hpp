#pragma once

#include <gmock/gmock.h>

#include "znet/iznet_provisioning_list_query_data_source.hpp"

namespace znet {
namespace ut {

class MockProvisioningListQueryDataSource : public ::znet::api::IProvisioningListQueryDataSource {
public:
    MOCK_METHOD(::znet::api::IProvisioningListController::OperationStatus, remove, ( uint32_t ), ( override ) );
    MOCK_METHOD(::znet::api::IProvisioningListController::OperationStatus,
                update,
                (const ProvisioningListEntry&),
                ( override ) );
    MOCK_METHOD(::znet::api::IProvisioningListController::OperationStatus,
                iterate,
                (const iterate_function_t&),
                ( const override ) );
};

}  // namespace ut
}  // namespace znet
