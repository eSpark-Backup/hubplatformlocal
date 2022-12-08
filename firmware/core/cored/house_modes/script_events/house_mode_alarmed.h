#pragma once

#include "cored/house_modes/script_events/base_event.h"

namespace HubPlatform {
namespace CoreDHouseModes {
namespace ScriptEvents {

class HouseModeAlarmed : public BaseEvent {
public:
    HouseModeAlarmed( std::string modeId, std::string deviceId );

    Json toJson() const override;

private:
    std::string m_modeId;
    std::string m_deviceId;
};

}  // namespace ScriptEvents
}  // namespace CoreDHouseModes
}  // namespace HubPlatform
