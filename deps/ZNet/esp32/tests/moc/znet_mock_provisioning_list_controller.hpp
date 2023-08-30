#pragma once

#include <gmock/gmock.h>

#include "znet/iznet_provisioning_list_controller.hpp"

namespace znet {
namespace ut {

class MockProvisioningListController : public ::znet::api::IProvisioningListController {
public:
    MOCK_METHOD( OperationStatus,
                 addProvisioningListEntry,
                 (const ::znet::DSK&, const ::znet::ProvisioningListEntryFields&),
                 ( override ) );
    MOCK_METHOD( OperationStatus,
                 setProvisioningListEntry,
                 (uint32_t provisioningId, const ::znet::ProvisioningListEntryFields&),
                 ( override ) );
    MOCK_METHOD( OperationStatus, removeProvisioningListEntry, ( uint32_t provisioningId ), ( override ) );
    MOCK_METHOD( OperationStatus, findProvisioningListEntry, (const ProcessProvisioningCallback&), ( const override ) );
};

}  // namespace ut
}  // namespace znet
