#pragma once

#include <gmock/gmock.h>

#include "znet/iznet_provisioning_list_provider.hpp"

namespace znet {
namespace ut {

class MockProvisioningListProvider : public ::znet::IProvisioningListProvider {
public:
    MOCK_METHOD( bool, storeProvisioningListEntry, (const ::znet::ProvisioningListEntry&), ( override ) );
    MOCK_METHOD( bool, removeProvisioningListEntryByID, ( uint32_t ), ( override ) );
    MOCK_METHOD( bool, findProvisioningListEntryByID, (uint32_t, ::znet::ProvisioningListEntry&), ( const override ) );
    MOCK_METHOD( bool,
                 findProvisioningListEntryByDSK,
                 ((const DSK&), ::znet::ProvisioningListEntry&),
                 ( const override ) );
    MOCK_METHOD( bool,
                 loadProvisioningList,
                 (const IProvisioningListProvider::ProcessProvisioningCallback&),
                 ( const override ) );
    MOCK_METHOD( size_t, getProvisioningListEntryCount, (), ( const override ) );
};

}  // namespace ut
}  // namespace znet
