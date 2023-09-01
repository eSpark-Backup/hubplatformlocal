#include "fake_owner_fixture.h"

namespace znet {
namespace ut {

FakeOwnerFixture::FakeOwnerFixture()
: FakeOwnerFixture( std::unique_ptr<FakeOwner>( new FakeOwner ) ) {}

FakeOwnerFixture::FakeOwnerFixture( std::unique_ptr<FakeOwner> owner )
: m_owner( std::move( owner ) )
, m_impl( *m_owner, m_classicInclusionSecurityDataProvider, m_provisioningListProvider, m_provisioningListObserver ) {
    m_owner->setRequestsManager( &m_impl.getRequestsManager() );
}

ZNetImpl& FakeOwnerFixture::getImpl() {
    return m_impl;
}

FakeOwner& FakeOwnerFixture::getOwner() {
    return *m_owner;
}

}  // namespace ut
}  // namespace znet
