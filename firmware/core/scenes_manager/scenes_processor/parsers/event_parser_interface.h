#pragma once

#include "scenes_manager/scenes_processor/id_utils.h"
#include "scenes_manager/scenes_engine/event_container.h"
#include "scenes_manager/scenes_engine/events/item_value_provider.h"
#include "scenes_manager/scenes_engine/value_source_key.h"

namespace SceneEngine {
class IClock;
}

namespace HubPlatform {

namespace Services {
class TaskLoop;
}

namespace ScenesManager {

class EventParserInterface {
public:
    using ControllerId = std::string;
    using GetGeoPositionCallback = SceneEngine::SunStateEvent::GetGeoPositionCallback;
    using MakeGenericValueProvider = std::function< SceneEngine::ItemValueProviderPtr( SceneEngine::ValueSourceKey ) >;

public:
    virtual ~EventParserInterface() = default;
    virtual SceneEngine::EventContainer parseTriggers( Json& sceneJsonObject,
                                                       IdUtils::PlaceholdersMapping& idsMapping ) = 0;
    virtual SceneEngine::EventContainer parseExceptions( Json& sceneJsonObject,
                                                         IdUtils::PlaceholdersMapping& idsMapping ) = 0;
    virtual void setClock( SceneEngine::IClock& clock ) = 0;
};

class EventParserFactory {
public:
    static std::unique_ptr< EventParserInterface > makeEventParser(
        EventParserInterface::ControllerId controllerId,
        EventParserInterface::GetGeoPositionCallback getGeoPosition,
        EventParserInterface::MakeGenericValueProvider makeGenericValueProvider,
        Services::TaskLoop& taskLoop,
        SceneEngine::IClock& clock );
};

}  // namespace ScenesManager
}  // namespace HubPlatform
