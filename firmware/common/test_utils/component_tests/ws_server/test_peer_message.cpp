
#include "test_utils/component_tests/ws_server/test_peer_message.h"

namespace HubPlatform {
namespace Tests {

TestPeerMessage::TestPeerMessage( TestServerPeer* pPeer )
: rpc::TypedServerMessage< TestServerPeer >( pPeer ) {}

TestPeerMessage::~TestPeerMessage() {}

std::string TestPeerMessage::getSerial() const {  // if not empty, then logged in
    tiny::retain_release_ptr< TestServerPeer > px = getPeerObject();

    if( !px )
        return "";

    return px->getSerial();
}

bool TestPeerMessage::isLoggedIn() const {
    return !getSerial().empty();
}

}  // namespace Tests
}  // namespace HubPlatform