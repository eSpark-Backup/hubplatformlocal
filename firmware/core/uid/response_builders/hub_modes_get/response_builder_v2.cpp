
#include "uid/response_builders/hub_modes_get/response_builder_v2.h"

namespace HubPlatform {
namespace Uid {
namespace ResponseBuilder {
namespace HubModesGet {
namespace V2 {

ResponseBuilder::ResponseBuilder( const rpc::Message& message, const std::string& api )
: V1::ResponseBuilder( message, api ) {}

JsonObject ResponseBuilder::buildResponse() const {
    auto joResponse = V1::ResponseBuilder::buildResponse();

    joResponse[ "result" ][ "timeIsLeftToSwitch" ] = m_timeToLeftForSwitching;
    joResponse[ "result" ][ "alarmDelay" ] = m_alarmOnDelay;
    joResponse[ "result" ][ "switchToDelay" ] = m_modesSwitchDelay;

    for( auto& item : joResponse[ "result" ][ "modes" ] ) {
        item.erase( "switchToDelay" );
        item.erase( "alarmDelay" );
    }

    return joResponse;
}

}  // namespace V2
}  // namespace HubModesGet
}  // namespace ResponseBuilder
}  // namespace Uid
}  // namespace HubPlatform