#pragma once
#include "scenes_manager/scenes_engine/operators/function_operator.h"
#include "service_interfaces/task_loop.h"

namespace SceneEngine {

class ForFunctionOperator : public FunctionOperator {
public:
    explicit ForFunctionOperator( HubPlatform::Services::TaskLoop& taskLoop );
    ~ForFunctionOperator();

    void setOptions( const std::string& method, std::chrono::milliseconds interval );
    OperatorState checkParent( SharedOperator child, OperatorState state ) override;
    OperatorState checkChildren() override;
    void accept( OperatorVisitor& visitor ) override;

public:
    void stopTimer();
    void startTimer();

protected:
    enum class Method : int { LESS, AT_LEAST };

    void saveTransitionMark();
    std::chrono::milliseconds diffFromNow() const;
    bool saveCurrentState( AbstractOperator::OperatorState newState );
    AbstractOperator::OperatorState getCurrentState() const;

private:
    Method m_method{ Method::LESS };
    std::chrono::milliseconds m_intervalMilliseconds{ 0 };
    AbstractOperator::OperatorState m_currentState{ OperatorState::False };
    std::chrono::time_point< std::chrono::system_clock > m_transition;

    HubPlatform::Services::TaskLoop& m_taskLoop;
    HubPlatform::Services::PostponedTaskPtr m_timer;
};

}  // namespace SceneEngine