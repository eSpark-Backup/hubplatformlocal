#pragma once
#include "http_bridge/cameras_addon_endpoint.h"
#include "service_interfaces/service.h"
#include <mutex>

namespace HubPlatform {
namespace Uid {

class CamerasEndpointHandler : public HttpBridge::ICamerasEndpointHandler {
public:
    CamerasEndpointHandler( Services::Service& hostService );
    virtual void getSnapshot( HttpBridge::CallbackParams params, const std::string& streamId ) override;
    virtual bool isHandlerValid() override;

private:
    bool isSessionClosed( struct lws* wsi ) const;
    Services::Service& m_hostService;

    static const int CameraAddonRequestTimeout = 60;
};

}  // namespace Uid
}  // namespace HubPlatform
