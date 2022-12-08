#pragma once

#include "znet_node_accessor.h"
#include "znet_impl_accessor.h"
#include "fake_owner.h"
#include "znet_mock_provisioning_list_observer.hpp"
#include "znet_mock_provisioning_list_provider.hpp"

namespace znet {
namespace ut {

class FakeOwnerFixture : public ZNodeAccessor, public ZNetImplAccessor {
public:
    FakeOwnerFixture();

    FakeOwnerFixture( std::unique_ptr<FakeOwner> owner );

    ZNetImpl& getImpl();

    FakeOwner& getOwner();

private:
    MockProvisioningListObserver m_provisioningListObserver;
    MockProvisioningListProvider m_provisioningListProvider;
    MockProvisioningListProvider m_classicInclusionSecurityDataProvider;
    std::unique_ptr<FakeOwner> m_owner;
    ZNetImpl m_impl;
};

}  // namespace ut
}  // namespace znet
