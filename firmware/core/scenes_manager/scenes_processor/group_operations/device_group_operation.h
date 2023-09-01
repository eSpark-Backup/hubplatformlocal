#pragma once
#include "json_utils/json_fwd.h"
#include "service_interfaces/service.h"

namespace HubPlatform {
namespace ScenesManager {

class DeviceGroupOperation {
public:
    explicit DeviceGroupOperation( Services::Service& service );
    JsonObject create( const JsonObject& params );
    JsonObject deleteGroup( const JsonObject& params );
    JsonObject update( const JsonObject& params );
    JsonObject list( const JsonObject& params );
    JsonObject get( const JsonObject& params );

private:
    Services::Service& m_hostService;
};

}  // namespace ScenesManager
}  // namespace HubPlatform
