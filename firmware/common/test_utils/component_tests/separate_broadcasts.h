#pragma once

#include "test_utils/component_tests/common_nma_test_fixture.h"

namespace HubPlatform {
namespace Tests {

class SeparateBroadcasts {
public:
    explicit SeparateBroadcasts( CommonNmaTests& commonNma, std::string broadcastName = "hub.extensions.plugin.run" );

    /** @brief Separates UI broadcasts from hub.software.update.progress broadcasts */
    Json waitForUiBroadcast( const std::string& message,
                             const char* file,
                             int line,
                             std::chrono::seconds timeout = BroadcastTimeout );

    /** @brief Separates hub.software.update.progress from other ui broadcasts */
    Json waitForProgressBroadcast( const std::string& message,
                                   const char* file,
                                   int line,
                                   std::chrono::seconds timeout = progressBroadcastTimeout );

private:
    std::mutex m_uiBroadcastsMutex;
    std::deque< JsonObject > m_uiBroadcastMessages;

    std::mutex m_progressBroadcastsMutex;
    std::deque< JsonObject > m_progressBroadcastMessages;

    CommonNmaTests& m_commonNma;
    const std::string m_broadcastName;
};

}  // namespace Tests
}  // namespace HubPlatform