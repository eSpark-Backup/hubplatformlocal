#pragma once
#include "scenes_manager/scenes_engine/actions/iscene_action_execute.h"

namespace HubPlatform {
namespace Services {
class Service;
}
namespace CoreDRemoteInterface {
class CoreDInterface;
}
}  // namespace HubPlatform

namespace SceneEngine {

class ControllerFactoryInterface;

class SceneActionExecuteFactory {
public:
    SceneActionExecuteFactory( HubPlatform::Services::Service& hostService,
                               HubPlatform::CoreDRemoteInterface::CoreDInterface& coreInterface,
                               ControllerFactoryInterface& controllerFactory,
                               BroadcastCallback sendBroadcast );

    SceneActionExecutePtr create( ActionBlocks&& actions, SceneSharedPtr scenePtr );

private:
    HubPlatform::Services::Service& m_hostService;
    HubPlatform::CoreDRemoteInterface::CoreDInterface& m_coreInterface;
    ControllerFactoryInterface& m_controllerFactory;
    BroadcastCallback m_sendBroadcast;
};

}  // namespace SceneEngine