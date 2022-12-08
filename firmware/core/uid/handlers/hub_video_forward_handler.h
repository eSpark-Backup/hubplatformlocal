#pragma once

#include "jsonrpc/message.h"

namespace HubPlatform {
namespace Uid {

class Service;

class HubVideoForwardHandler final {
public:
    HubVideoForwardHandler( Uid::Service& hostService );

    void handleVideoForward( rpc::Message& rpcMessage );

private:
    Uid::Service& m_service;
};

}  // namespace Uid
}  // namespace HubPlatform
