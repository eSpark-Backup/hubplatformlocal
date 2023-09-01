#pragma once

#include "networkd/remote_interface/network_remote_interface.h"

#include "jsonrpc/message.h"

#include <string>

namespace HubPlatform {
namespace Uid {

class HubNetworkCommandsHandler final {
public:
    HubNetworkCommandsHandler( Services::Service& hostService );

    void invoke( const std::string& method, rpc::Message& rpcMessage );

private:
    void handleGetRequest( rpc::Message& rpcMessage );
    void handleSettingsSetRequest( rpc::Message& rpcMessage );
    void handleResetRequest( rpc::Message& rpcMessage );
    void handleWifiScanStartRequest( rpc::Message& rpcMessage );
    void handleWifiScanStopRequest( rpc::Message& rpcMessage );
    void handleWifiTryConnectRequest( rpc::Message& rpcMessage );
    void handleModemTryConnectRequest( rpc::Message& rpcMessage );
    void handleModemPinCodeSetRequest( rpc::Message& rpcMessage );
    void handleModemPinCodeResetRequest( rpc::Message& rpcMessage );
    void handleModemSimLockSetRequest( rpc::Message& rpcMessage );
    void handleModemClearFPLMNRequest( rpc::Message& rpcMessage );
    void handleModemReRegisterRequest( rpc::Message& rpcMessage );
    void handleStatisticsSettingsGetRequest( rpc::Message& rpcMessage );
    void handleStatisticsSettingsSetRequest( rpc::Message& rpcMessage );
    void handleStatisticsGetRequest( rpc::Message& rpcMessage );
    void handleEnabledSetRequest( rpc::Message& rpcMessage );

private:
    Network::RemoteInterface m_networkdInterface;
};

}  // namespace Uid
}  // namespace HubPlatform
