#pragma once

#include "test_utils/component_tests/component_test_application.h"
#include "test_utils/component_tests/gateways_checker.h" //< EFWR-3896 >

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
    std::unique_ptr< GatewaysChecker > m_gatewaysChecker; //< EFWR-3896 >
};

}  // namespace Tests
}  // namespace HubPlatform
