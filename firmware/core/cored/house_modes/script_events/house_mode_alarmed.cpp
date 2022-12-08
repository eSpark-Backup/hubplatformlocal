#include "cored/house_modes/script_events/house_mode_alarmed.h"
#include "cored/house_modes/resources/house_mode_resources.h"

namespace HubPlatform {
namespace CoreDHouseModes {
namespace ScriptEvents {

HouseModeAlarmed::HouseModeAlarmed( std::string modeId, std::string deviceId )
: BaseEvent( Resources::ScriptEvents::Name::HouseModeAlarmed )
, m_modeId( std::move( modeId ) )
, m_deviceId( std::move( deviceId ) ) {}

Json HouseModeAlarmed::toJson() const {
    auto result = BaseEvent::toJson();

    result[ "house_mode_id" ] = m_modeId;
    result[ "device_id" ] = m_deviceId;

    return result;
}

}  // namespace ScriptEvents
}  // namespace CoreDHouseModes
}  // namespace HubPlatform
