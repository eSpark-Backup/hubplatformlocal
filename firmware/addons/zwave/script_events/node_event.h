#pragma once

#include "service_interfaces/script_event.h"

namespace HubPlatform {
namespace Zwave {

class NodeEvent : public Services::ScriptEvent {
public:
    NodeEvent( std::string eventName, uint8_t nodeId );

    Json toJson() const override;

private:
    uint8_t m_nodeId;
};

}  // namespace Zwave
}  // namespace HubPlatform
