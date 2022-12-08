#include "scenes_manager/scenes_engine/operators/for_function_operator.h"
#include "scenes_manager/scenes_engine/exceptions.h"
#include "tiny/tiny_consolecolors.h"
#include "log_utils/ezlo_log.h"

namespace SceneEngine {

ForFunctionOperator::ForFunctionOperator( HubPlatform::Services::TaskLoop& taskLoop )
: m_taskLoop( taskLoop ) {}

ForFunctionOperator::~ForFunctionOperator() {
    stopTimer();
}

void ForFunctionOperator::setOptions( const std::string& method, std::chrono::milliseconds interval ) {
    if( method == "less" ) {
        m_method = Method::LESS;
    }
    else if( method == "least" ) {
        m_method = Method::AT_LEAST;
    }
    else {
        throw SceneException( Errors::SCENE_WRONG_WHEN_BLOCK,
                              std::string{ "Invalid method of FOR function: " }.append( method ).c_str() );
    }

    m_intervalMilliseconds = interval;
    setInterceptSceneState( true );
}

AbstractOperator::OperatorState ForFunctionOperator::checkParent( AbstractOperator::SharedOperator child,
                                                                  AbstractOperator::OperatorState state ) {
    // check enabled and empty
    if( !isEnabled() || getChildren().empty() ) {
        return OperatorState::Disabled;
    }

    auto childrenState = checkChildren();
    AbstractOperator::OperatorState changedChildrenState = childrenState;

    // Evaluate if child state change affects the whole operator state change
    for( const auto& condition : getChildren() ) {
        if( condition == child ) {
            if( state == OperatorState::False ) {
                // modify state of a child
                AbstractOperator::checkParent( child, OperatorState::False );
                // check if state of all children is changed
                changedChildrenState = checkChildren();
            }
            break;
        }
    }

    // False case
    if( changedChildrenState == OperatorState::False ) {
        // state transition to false
        auto prevState = getCurrentState();

        if( prevState != changedChildrenState ) {
            auto timeDiff = diffFromNow();
            saveCurrentState( OperatorState::False );

            if( m_method == Method::LESS ) {
                if( std::chrono::duration_cast< std::chrono::milliseconds >( timeDiff ) < m_intervalMilliseconds ) {
                    // Change state to True, and immediately to False after that
                    notifyOperatorStateEvent( OperatorState::True );
                    auto sceneState = AbstractOperator::checkParent( child, OperatorState::True );
                    LOGGER_DEBUG( cc::debug( "for/less: OK, sceneState = " + cc::flag( toBool( sceneState ) ) ) );
                    notifySceneStateEvent( sceneState );

                    // Change state  immediately to False after that
                    notifyOperatorStateEvent( OperatorState::False );
                    auto sceneState2 = AbstractOperator::checkParent( child, OperatorState::False );
                    notifySceneStateEvent( sceneState2 );

                    return OperatorState::False;
                }
                else {
                    LOGGER_TRACE( cc::trace( "for/less: Not OK" ) );
                }
            }
            else if( m_method == Method::AT_LEAST ) {
                LOGGER_TRACE( cc::trace( "for/least: False, stop timer" ) );
                stopTimer();
                auto sceneState2 = AbstractOperator::checkParent( child, OperatorState::False );
                notifySceneStateEvent( sceneState2 );
            }
        }
    }

    // True case
    // Do not take in account the value of child state, but only the value of all children
    if( toBool( changedChildrenState ) && m_method == Method::AT_LEAST ) {
        bool isStateChanged = saveCurrentState( changedChildrenState );

        if( isStateChanged ) {
            stopTimer();

            // Change scene state to true if
            auto sceneState2 = AbstractOperator::checkParent( child, OperatorState::False );
            notifySceneStateEvent( sceneState2 );

            LOGGER_TRACE( cc::trace( "for/least: start new timer" ) );
            startTimer();
            return OperatorState::False;
        }
        else if( !isStateChanged ) {
            if( m_timer != nullptr ) {
                // timer is present, return True to activate a scene
                LOGGER_DEBUG( cc::debug( "for/least: OK, sceneState = " + cc::flag( true ) ) );
                return OperatorState::True;
            }
        }
    }
    // Do not take in account the value of child state, but only the value of all children
    else if( toBool( changedChildrenState ) && m_method == Method::LESS ) {
        saveCurrentState( changedChildrenState );
        return OperatorState::False;
    }
    return OperatorState::False;
}

AbstractOperator::OperatorState ForFunctionOperator::checkChildren() {
    return AndOperator::checkChildren();
}

void ForFunctionOperator::saveTransitionMark() {
    m_transition = std::chrono::system_clock::now();
}

std::chrono::milliseconds ForFunctionOperator::diffFromNow() const {
    auto now = std::chrono::system_clock::now();
    auto millisecondsPassed = std::chrono::duration_cast< std::chrono::milliseconds >( now - m_transition );
    return millisecondsPassed;
}

bool ForFunctionOperator::saveCurrentState( AbstractOperator::OperatorState newState ) {
    auto prevState = m_currentState;
    m_currentState = newState;
    if( prevState != m_currentState ) {
        auto diff = diffFromNow();
        LOGGER_TRACE( cc::trace( "transition occurred: " ) + cc::flag( toBool( newState ) ) +
                      cc::trace( ", delta from previous: " ) + cc::num10( diff.count() ) + cc::trace( " ms" ) );
        saveTransitionMark();

        return true;
    }
    return false;
}

AbstractOperator::OperatorState ForFunctionOperator::getCurrentState() const {
    return m_currentState;
}

void ForFunctionOperator::stopTimer() {
    if( m_timer ) {
        m_timer->stop();
        m_timer.reset();
    }
}

void ForFunctionOperator::startTimer() {
    using namespace HubPlatform::Services;
    auto forOperator = std::static_pointer_cast< ForFunctionOperator >( shared_from_this() );
    m_timer = m_taskLoop.createPostponedTask( [forOperator]( PostponedTask& ) {
        auto sceneState = forOperator->checkParent( forOperator, OperatorState::True );
        forOperator->notifyOperatorStateEvent( OperatorState::True );
        LOGGER_DEBUG_RAW( cc::debug( "for/least timer: notifySceneStateEvent called, sceneState = " ) +
                          cc::flag( toBool( sceneState ) ) );
        forOperator->notifySceneStateEvent( sceneState );
        forOperator->stopTimer();
    } );

    m_timer->start( TaskLoop::Interval( m_intervalMilliseconds ), false );
}

void ForFunctionOperator::accept( OperatorVisitor& visitor ) {
    visitor.visit( *this );
}

}  // namespace SceneEngine
