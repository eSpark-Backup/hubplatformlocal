#pragma once

#include <string>
#include "json_utils/json_utils.h"
#include "log_utils/ezlo_log.h"

namespace HubPlatform {
namespace ScenesManager {

class GroupInfo {
public:
    GroupInfo() = default;

public:
    virtual Json toJson() const = 0;
    virtual bool fromJson( const JsonObject& data ) = 0;
};

}  // namespace ScenesManager
}  // namespace HubPlatform
