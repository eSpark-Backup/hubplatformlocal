#pragma once

#include <atomic>

class Effect {
public:
    Effect();
    virtual ~Effect() = default;
    void start();
    void stop();

protected:
    bool isRunning() const;
    virtual void effect() = 0;

private:
    std::atomic_bool m_shouldBeStopped;
    std::atomic_bool m_stopRequestHandled;
};
