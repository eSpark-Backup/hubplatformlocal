#pragma once

#include <gmock/gmock.h>

#include "znet/iznet_provisioning_list_observer.hpp"

namespace znet {
namespace ut {

class MockProvisioningListObserver : public ::znet::IProvisioningListObserver {
public:
    MOCK_METHOD( void, onProvisioningListEntryAdded, (const ::znet::ProvisioningListEntry&), ( override ) );
    MOCK_METHOD( void, onProvisioningListEntryRemoved, ( uint32_t ), ( override ) );
    MOCK_METHOD( void,
                 onProvisioningListEntryUpdated,
                 (const ::znet::ProvisioningListEntry&, const ::znet::ProvisioningListEntry&),
                 ( override ) );
};

}  // namespace ut
}  // namespace znet
