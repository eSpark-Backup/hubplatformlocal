#include "scenes_manager/tests/fixtures/scenes_api_fixture.h"

namespace HubPlatform {
namespace ScenesManager {
class ScenesRequestHandler;
}

namespace UT {

class GroupOperationsFixture : public ScenesAPIFixture {
protected:
    void addDevice( const std::string& deviceId );
    void addItemGroup( const std::string& groupId, const std::string& name );
    void addDeviceGroup( const std::string& groupId, const std::string& name );
    JsonObject getDeviceGroupsList( const JsonObject& params );
    JsonObject getDeviceGroupGet( const JsonObject& params );
    JsonObject validateDeviceGroupCreate( JsonObject& message );
    JsonObject validateItemGroupCreate( JsonObject& message );
    JsonObject getDeviceGroupUpdateBroadcastData( const JsonObject& updated, const JsonObject& saved );
    JsonObject getItemGroupUpdateBroadcastData( const JsonObject& updated, const JsonObject& saved );
    JsonObject getItemGroupsList( const JsonObject& params );
    JsonObject getItemGroupGet( const JsonObject& params );
};
}  // namespace UT
}  // namespace HubPlatform
