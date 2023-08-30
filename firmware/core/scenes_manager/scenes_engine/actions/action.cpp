#include "scenes_manager/scenes_engine/actions/action.h"
#include "scenes_manager/scenes_engine/actions/dispatcher.h"
#include "scenes_manager/scenes_engine/exceptions.h"
#include <utility>

namespace SceneEngine {

Action::Action( InitData&& initData )
: m_name( std::move( initData.name ) )
, m_id( initData.id )
, m_arguments( std::move( initData.arguments ) )
, m_delay( initData.delay )
, m_executionPolicy( initData.executionPolicy )
, m_subscriptionState( SubscriptionState::No )
, m_controllerId( std::move( initData.controllerId ) )
, m_exceptionData( std::move( initData.exceptionData ) )
, m_isInitialized( true ) {}

void Action::requestRun( ActionExecutorPtr callback ) {
    Dispatcher::instance().fireAction( m_name, *this, std::move( callback ) );
}

const std::string& Action::getName() const {
    return m_name;
}

Action::Id Action::getId() const {
    return m_id;
}

const Action::Arguments& Action::getArgs() const {
    return m_arguments;
}

const Action::Delay& Action::getDelay() const {
    return m_delay;
}

ExecutionPolicy Action::getExecutionPolicy() const {
    return m_executionPolicy;
}

const Action::ControllerId& Action::getControllerId() const {
    return m_controllerId;
}

const SubscriptionState& Action::getSubscriptionState() const {
    return m_subscriptionState;
}

const ActionExceptionData& Action::getExceptionData() const {
    return m_exceptionData;
}

bool Action::isInitialized() const {
    return m_isInitialized;
}

void Action::setArgs( Arguments args ) {
    m_arguments = std::move( args );
}

void Action::setSubscriptionState( SubscriptionState state ) {
    m_subscriptionState = state;
}

void Action::setInitialized( bool isInitialized ) {
    m_isInitialized = isInitialized;
}

}  // namespace SceneEngine
