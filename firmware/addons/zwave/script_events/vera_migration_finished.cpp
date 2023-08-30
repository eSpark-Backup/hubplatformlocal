#include "zwave/script_events/vera_migration_finished.h"

#include "zwave/resources/zwave_resources.h"

namespace HubPlatform {
namespace Zwave {

VeraMigrationFinished::VeraMigrationFinished( Json&& veraDevicesModel )
: Services::ScriptEvent( Resources::ScriptEvent::Name::VeraMigrationFinished )
, m_veraDevicesModel( std::move( veraDevicesModel ) ) {}

Json VeraMigrationFinished::toJson() const {
    using namespace Resources::ScriptEvent;

    auto result = Services::ScriptEvent::toJson();

    result[ Param::VeraDeviceModel ] = m_veraDevicesModel;

    return result;
}

}  // namespace Zwave
}  // namespace HubPlatform
