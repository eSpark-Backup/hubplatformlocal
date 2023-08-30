#pragma once

#include "scenes_manager/scenes_engine/actions/iaction_executor.h"
#include "scenes_manager/scenes_engine/actions/action_exception_data.h"
#include <chrono>
#include <vector>

namespace SceneEngine {

enum class SubscriptionState { No, Item, Device };

class Action {
public:
    using Arguments = JsonObject;
    using Delay = std::chrono::milliseconds;
    using Id = int;
    using ControllerId = std::string;

    struct InitData {
        std::string name;
        Id id;
        Arguments arguments;
        ExecutionPolicy executionPolicy;
        ControllerId controllerId;
        ActionExceptionData exceptionData;
        Delay delay{ 0 };
    };

public:
    explicit Action( InitData&& initData );
    virtual ~Action() = default;

    virtual void requestRun( ActionExecutorPtr callback );

    const std::string& getName() const;
    Id getId() const;
    const Arguments& getArgs() const;
    const Delay& getDelay() const;
    ExecutionPolicy getExecutionPolicy() const;
    const ControllerId& getControllerId() const;
    const SubscriptionState& getSubscriptionState() const;
    const ActionExceptionData& getExceptionData() const;
    bool isInitialized() const;

    void setArgs( Arguments args );
    void setSubscriptionState( SubscriptionState state );
    void setInitialized( bool isInitialized );

private:
    std::string m_name;
    Id m_id;
    Arguments m_arguments;
    Delay m_delay;
    ExecutionPolicy m_executionPolicy;
    SubscriptionState m_subscriptionState;
    ControllerId m_controllerId;
    ActionExceptionData m_exceptionData;
    bool m_isInitialized;
};

}  // namespace SceneEngine
