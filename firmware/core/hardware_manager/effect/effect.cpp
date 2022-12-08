#include "hardware_manager/effect/effect.h"

Effect::Effect()
: m_shouldBeStopped( false )
, m_stopRequestHandled( false ) {}

void Effect::start() {
    if( !m_shouldBeStopped || m_stopRequestHandled ) {
        effect();
    }
    m_stopRequestHandled = true;
    m_shouldBeStopped = false;
}

void Effect::stop() {
    m_shouldBeStopped = true;
}

bool Effect::isRunning() const {
    return !m_shouldBeStopped;
}
