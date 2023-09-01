#pragma once
#include "scenes_manager/scenes_engine/events/item_event.h"
#include "scenes_manager/scenes_engine/actions/action.h"
#include "scenes_manager/scenes_engine/controller_interface/controller_factory_interface.h"
#include "scenes_manager/scenes_processor/http_dispatcher.h"
#include "scenes_manager/scenes_engine/expressions/expression_manager.h"
#include "scenes_manager/scenes_processor/scene_request_handler_interface.h"
#include "scene_actions.h"

#include <string>
#include <map>
#include <thread>
#include <memory>
#include <mutex>

namespace HubPlatform {

namespace Services {
class Service;
}  // namespace Services

namespace ScenesManager {

namespace HttpCodes {
static constexpr int Ok = 200;
static constexpr int RequestTimeout = 408;
static constexpr int ClientClosedRequest = 499;
static constexpr int InternalServerError = 500;
static constexpr int NotFound = 404;
static constexpr int ServiceUnavailable = 503;
static constexpr int NetworkAuthenticationRequired = 511;
}  // namespace HttpCodes

class ActionInterface {
public:
    using ActionExecutorPtr = SceneEngine::ActionExecutorPtr;
    using ActionArguments = SceneEngine::Action::Arguments;
    using Action = SceneEngine::Action;

    class SendFinishCallback {
    public:
        SendFinishCallback( const Action& action, ActionExecutorPtr finishCallback )
        : m_action( action )
        , m_actionExecutorPtr( finishCallback ) {}
        ~SendFinishCallback() {
            if( m_actionExecutorPtr &&
                m_actionExecutorPtr->setResult(
                    { m_action.getId(), m_action.getExecutionPolicy(), m_result, m_message } ) ) {
                if( m_action.getExecutionPolicy() == SceneEngine::ExecutionPolicy::WaitResult ) {
                    m_actionExecutorPtr->runNextAction();
                }
                else if( m_result && m_action.getExecutionPolicy() == SceneEngine::ExecutionPolicy::CheckResult ) {
                    m_actionExecutorPtr->runNextAction();
                }
            }
        }
        void setResult( bool isSuccessful ) {
            m_result = isSuccessful;
        }
        void setResult( const Json& message ) {
            m_message = message;
        }

    private:
        bool m_result = false;
        Json m_message;
        Action m_action;
        ActionExecutorPtr m_actionExecutorPtr;
    };
    virtual ~ActionInterface() = default;
    virtual void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) = 0;
};

class ControllerCoreActions : public ActionInterface {
public:
    using ControllerFactory = SceneEngine::ControllerFactoryInterface;

    ControllerCoreActions( ControllerFactory& controllerFactory )
    : m_controllerFactory( controllerFactory ) {}

protected:
    ControllerFactory& m_controllerFactory;
};

class SetItemValue : public ControllerCoreActions {
public:
    SetItemValue( ControllerFactory& controllerFactory );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;
};

class DeviceArmed : public ControllerCoreActions {
public:
    DeviceArmed( ControllerFactory& controllerFactory );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;
};

/**
 * @brief
 * "Template method" implementation of ActionInterface
 * Performs a one-shot async request to the host service
 * Delegates parsing of action arguments (getRequestData method)
 */
class HostRequestAction : public ActionInterface {
public:
    explicit HostRequestAction( Services::Service& hostService );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr );

protected:
    struct RequestData {
        std::string m_serviceName;
        MessageSubClasses m_requestType;
        JsonObject m_requestBody;
    };
    class InvalidActionArgs : public std::invalid_argument {
    public:
        using std::invalid_argument::invalid_argument;
    };

    virtual RequestData getRequestData( const Action::Arguments& args ) = 0;

    Services::Service& m_hostService;
};

class SendCloudAbstractCommand : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class SwitchHouseMode : public ControllerCoreActions {
public:
    SwitchHouseMode( ControllerFactory& controllerFactory );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;
};

class GlobalVariable;

class SendHttpRequest : public ActionInterface {
public:
    using Callback = std::shared_ptr< SendFinishCallback >;

    SendHttpRequest( Services::Service& hostService );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;

private:
    using CallbacksMapping = std::map< HttpDispatcher::RequestId, Callback >;
    using VariablesMapping = std::map< HttpDispatcher::RequestId, std::unique_ptr< GlobalVariable > >;

    static void processResponse( const std::string& eventHandler, const CommonHttp::EventPtr& event );
    static void processTimeout( const std::string& eventHandler, const CommonHttp::EventPtr& event );

    static Callback extractCallback( const HttpDispatcher::RequestId& requestId );
    static std::unique_ptr< GlobalVariable > extractVariable( const HttpDispatcher::RequestId& requestId );

private:
    Services::Service& m_hostService;
    static std::mutex m_mutex;
    static CallbacksMapping m_callbacks;
    static VariablesMapping m_variables;

    static const int ResponseTimeout = 60 * 1000;
    static const int ConnectionTimeout = 30 * 1000;
    static const int RedirectionNumber = 10;
};

class RunLuaCodeAction final : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class RunLuaPluginScriptAction final : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class RunSceneAction final : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class SetSceneStateAction final : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class ResetLatchAction : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class ResetSceneLatchesAction : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class RebootHub final : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class CloudAPICommand : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class ResetHubAction : public ActionInterface {
public:
    explicit ResetHubAction( Services::Service& hostService );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;

private:
    Services::Service& m_hostService;
};

class SetExpressionAction : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class SetVariableAction : public HostRequestAction {
public:
    using HostRequestAction::HostRequestAction;
    RequestData getRequestData( const Action::Arguments& args ) override;
};

class ToggleValueAction : public ControllerCoreActions {
public:
    ToggleValueAction( ControllerFactory& controllerFactory,
                       SceneEngine::Expressions::ExpressionManager& expressionManager,
                       ScenesRequestHandlerInterface& scenesHandler );
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;

private:
    SceneEngine::Expressions::ExpressionManager& m_expressions;
    ScenesRequestHandlerInterface& m_scenesHandler;

    void actionToggleExpression( std::shared_ptr< ActionInterface::SendFinishCallback >& sendFinishCallbackPtr,
                                 const Action::Arguments& arguments );

    void actionToggleItem( const Action& action,
                           std::shared_ptr< ActionInterface::SendFinishCallback >& sendFinishCallbackPtr,
                           const SceneEngine::Action::Arguments& arguments );
};

class UnknownAction : public ActionInterface {
public:
    void run( const Action& action, ActionExecutorPtr actionExecutorPtr ) override;
};

class GlobalVariable {
public:
    static void parseSaveResult( const SceneEngine::Action::Arguments& args,
                                 std::string& name,
                                 std::string& valueType,
                                 std::string& method );

    GlobalVariable( Services::Service& hostService, const std::string& name, const std::string& typeValue );
    void setHttpError( Json& reason, SendHttpRequest::Callback callback );
    void setHTTPData( Json& headers, Json& value, SendHttpRequest::Callback callback );
    void setCloudApiData( const Json& message, SendHttpRequest::Callback callback );
    void setCloudApiError( const Json& error, SendHttpRequest::Callback callback );

private:
    Services::Service& m_hostService;
    std::string m_name;
    std::string m_valueType;
};

namespace SceneActions {
using ActionInterfacePtr = std::shared_ptr< ActionInterface >;
ActionInterfacePtr getActionForMethod( const std::string& method_name,
                                       Services::Service& hostService,
                                       SceneEngine::ControllerFactoryInterface& controllerFactory,
                                       SceneEngine::Expressions::ExpressionManager& expressionManager,
                                       ScenesRequestHandlerInterface& scenesHandler );
}  // namespace SceneActions

}  // namespace ScenesManager
}  // namespace HubPlatform
