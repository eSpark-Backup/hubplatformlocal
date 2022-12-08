#pragma once

#include "scenes_manager/scenes_engine/actions/iscene_action_execute.h"
#include "scenes_manager/scenes_engine/operators/scene_operator.h"
#include <vector>
#include <mutex>

namespace SceneEngine {

class ActionRunRequestsContainer {
public:
    void push( SceneActionExecutePtr executor, AbstractOperator::OperatorState state );
    void push( SceneActionExecutePtr executor, std::string blockId, AbstractOperator::OperatorState state );
    void runActions();

private:
    struct Entry {
        SceneActionExecutePtr executor;
        std::string blockId;
        AbstractOperator::OperatorState state;

        bool operator<( const Entry& other ) const;
    };

    std::mutex m_entriesMutex;
    std::vector< Entry > m_entries;
};

}  // namespace SceneEngine
