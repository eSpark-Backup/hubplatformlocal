#pragma once

#include "json_utils/json_fwd.h"

namespace HubPlatform {
namespace ScenesManager {

class ItemGroupUpdater {
public:
    static JsonObject getItemGroupUpdateBroadcastData( const JsonObject& updatedData, const JsonObject& savedData );
    static JsonObject getItemGroupUpdateData( const JsonObject& dbItem, const Json& params );
};

}  // namespace ScenesManager
}  // namespace HubPlatform
