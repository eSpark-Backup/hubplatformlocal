#pragma once
#include "scenes_manager/scenes_engine/helpers/json_helper.h"
#include "json_utils/json_path.h"
class IDbClient;
namespace HubPlatform {
namespace ScenesManager {
namespace ScenesGroupOperationsUtils {

bool checkStringParam( JsonObject& answer,
                       const JsonObject& params,
                       const std::string& key,
                       JsonPath& path,
                       bool required = true );
bool checkBoolParam( JsonObject& answer,
                     const JsonObject& params,
                     const std::string& key,
                     JsonPath& path,
                     bool required );
bool checkArrayParam( JsonObject& answer, const JsonObject& params, const std::string& key, JsonPath& path );
bool checkStringArrayParam( JsonObject& answer,
                            const JsonObject& params,
                            const std::string& key,
                            JsonPath& path,
                            bool required );

bool areObjectArraysEqual( const JsonArray& leftArray, const JsonArray& rightArray );
bool areStringArraysEqual( const JsonArray& leftArray, const JsonArray& rightArray );

}  // namespace ScenesGroupOperationsUtils
}  // namespace ScenesManager
}  // namespace HubPlatform
