#pragma once

#include "test_utils/component_tests/component_test_application.h"

namespace HubPlatform {
namespace Tests {

class CloudDevicesTestsApplication : public ComponentTestApplication {
public:
    CloudDevicesTestsApplication();

protected:
    void beforeApplicationsStartup() override;
    void setUpAdditionalApplications() override;
    void afterApplicationsStartup() override;

    bool shouldUseRandomLocalPort() override;

private:
    std::mutex m_requestsMutex;
    std::condition_variable m_requestsCondVar;
    std::deque< JsonObject > m_requestsMessages;
};

}  // namespace Tests
}  // namespace HubPlatform
