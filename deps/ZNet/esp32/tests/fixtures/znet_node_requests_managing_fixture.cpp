#include "znet_node_requests_managing_fixture.h"

namespace znet {
namespace ut {

NodeRequestsManagingFixture::NodeRequestsManagingFixture()
: m_requestsManager( m_timerProvider )
, m_nodesProvider( &m_requestsManager ) {}

FakeTimerProvider& NodeRequestsManagingFixture::getTimerProvider() {
    return m_timerProvider;
}

FakeRequestsManager& NodeRequestsManagingFixture::getRequestsManager() {
    return m_requestsManager;
}

FakeNodesProvider& NodeRequestsManagingFixture::getNodesProvider() {
    return m_nodesProvider;
}

RequestValue NodeRequestsManagingFixture::createRequestValue(
    ::testing::MockFunction<void( RequestFailTypeOpt )>& callback,
    WeakJobContext context ) {
    RequestValue request;
    request.jobContext = context;
    request.resultCallback = callback.AsStdFunction();
    return request;
}

}  // namespace ut
}  // namespace znet
