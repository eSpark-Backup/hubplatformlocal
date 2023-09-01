// Extracted from action.h/cpp
#include "service_interfaces/service.h"
#include "service_interfaces/postponed_task.h"
#include "scenes_manager/scenes_engine/actions/action_executor.h"
#include "scenes_manager/scenes_engine/actions/action.h"
#include <utility>

namespace SceneEngine {

ActionExecutor::ActionExecutor( HubPlatform::Services::Service& hostService,
                                std::string sceneId,
                                BroadcastCallback sendBroadcast,
                                JsonObject broadcast )
: m_hostService( hostService )
, m_sceneId( std::move( sceneId ) )
, m_sendBroadcast( std::move( sendBroadcast ) )
, m_broadcast( std::move( broadcast ) ) {}

ActionExecutor::~ActionExecutor() {
    if( !m_sendBroadcast || m_broadcast.empty() ) {
        return;
    }
    m_broadcast[ "status" ] = "finished";
    std::size_t failedActionQuantity = 0;
    for( const auto& action : m_resultList ) {
        if( action.result != false )
            continue;

        if( action.executionPolicy == ExecutionPolicy::CheckResult ) {
            m_broadcast[ "status" ] = "failed";
            break;
        }
        if( action.executionPolicy == ExecutionPolicy::IgnoreResult ||
            action.executionPolicy == ExecutionPolicy::WaitResult ) {
            m_broadcast[ "status" ] = "partially_finished";
            ++failedActionQuantity;
        }
    }
    if( m_actionQuantity > 0 && m_actionQuantity == failedActionQuantity ) {
        m_broadcast[ "status" ] = "failed";
    }
    m_sendBroadcast( m_broadcast );
}

bool ActionExecutor::setResult( const ActionResult& result ) {
    m_resultList.push_back( result );
    return true;
}

void ActionExecutor::setActions( const ActionList& actions ) {
    m_actions = actions;
    m_actionQuantity = actions.size();
}

void ActionExecutor::setActions( ActionExecutor::ActionList&& actions ) {
    m_actionQuantity = actions.size();
    m_actions = std::move( actions );
}

void ActionExecutor::runNextAction() {
    using namespace HubPlatform::Services;
    while( !m_actions.empty() ) {
        auto action = m_actions.front();
        auto executor = shared_from_this();
        auto runner = [action, executor]( PostponedTask& ) { action->requestRun( executor ); };

        PostponedTaskPtr timer = m_hostService.createPostponedTask( runner );
        timer->start( action->getDelay(), /*isPeriodic*/ false );

        m_actions.pop_front();
        if( action->getExecutionPolicy() != ExecutionPolicy::IgnoreResult ) {
            break;
        }
    }
}

void ActionExecutor::setSceneId( std::string sceneId ) {
    m_sceneId = std::move( sceneId );
}

const std::string& ActionExecutor::getSceneId() const {
    return m_sceneId;
}

}  // namespace SceneEngine
