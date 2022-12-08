#pragma once

#include "service_interfaces/script_event.h"

namespace HubPlatform {
namespace Zwave {

class VeraMigrationFinished : public Services::ScriptEvent {
public:
    VeraMigrationFinished( Json&& veraDevicesModel );

    Json toJson() const final;

private:
    Json m_veraDevicesModel;
};

}  // namespace Zwave
}  // namespace HubPlatform
