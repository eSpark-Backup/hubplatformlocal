
#include "cored/core_service.h"
#include "cored/model/resources/cored_resources.h"
#include "cored/remote_interface/cored_remote_interface.h"
#include "cored/tests/service/test_core_service.h"

#include "service_interfaces/bus_wrapper.h"
#include "service_interfaces/tests/fakes/fake_response_bus.h"
#include "cored/features_manager.cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

/*
TESTPLAN

DONE    1. Check protect_delayed feature
DONE    2. Check protect feature status
DONE    3. Check extra features
*/

namespace HubPlatform {
namespace UT {

using namespace Services;
using namespace CoreD::Model::Resources;

class FeatureManagerTestSuite : public ::testing::Test {
public:
    using KeyJsonList = std::vector< std::pair< std::string, JsonObject > >;

public:
    FeatureManagerTestSuite()
    : m_cored( std::make_unique< Services::BusWrapper >(
          Services::BusWrapper::BusTypeTag< Services::UT::FakeResponseBus >{} ) ) {
        // Do not print errors and warnings on erroneous test cases
        // TODO: implement fake log and check error messages too.
        Log::setLocalLogLevel( Log::TRACE );
    }

    CoreD::CoreService& getCoreDService() {
        return m_cored;
    }

    IDbClient& getDBClient() {
        return getCoreDService().getDBClient();
    }

    void cleanDB() {
        getDBClient().clean();
    }

protected:
    CoreD::Tests::TestCoreServiceWithoutHouseModes m_cored;
};

TEST_F( FeatureManagerTestSuite, checkProtectDelayedFeature ) {
    using namespace CoreD;
    const char* const featureProtectDelayed = "protect_delayed";
    FeaturesManager features = FeaturesManager( getDBClient() );
    CoreDRemoteInterface::CoreDRemoteInterface coreDInterface( getCoreDService() );
    auto feature = coreDInterface.getFeatures();
    auto featurePtr = coreDInterface.findFeature( featureProtectDelayed );

    ASSERT_TRUE( featurePtr.get() != nullptr );
    ASSERT_TRUE( coreDInterface.checkFeatureStatus( featureProtectDelayed ) );
    ASSERT_TRUE( featurePtr->getStatus() );
    ASSERT_FALSE( featurePtr->isEditable() );
}

TEST_F( FeatureManagerTestSuite, checkProtectFeatureStatus ) {
    using namespace CoreD;
    const char* const featureProtect = "protect";
    const bool statusExpected = false;
    getDBClient().insert( DBNames::FeaturesDB,
                          JSON( db::ID_FIELD_NAME << "protect"
                                                  << "version"
                                                  << "1.0"
                                                  << "status" << statusExpected << "editable"
                                                  << "false" ) );

    FeaturesManager features = FeaturesManager( getDBClient() );
    CoreDRemoteInterface::CoreDRemoteInterface core( getCoreDService() );
    auto featurePtr = core.findFeature( featureProtect );

    ASSERT_TRUE( featurePtr.get() != nullptr );
    ASSERT_EQ( core.checkFeatureStatus( featureProtect ), statusExpected );
    ASSERT_TRUE( featurePtr->isEditable() );
}

TEST_F( FeatureManagerTestSuite, checkExtraFeatures ) {
    using namespace CoreD;
    const char* const Test1 = "feature1";
    const char* const Test2 = "feature2";
    getDBClient().insert( DBNames::FeaturesDB,
                          JSON( db::ID_FIELD_NAME << Test1 << "version"
                                                  << "1.0"
                                                  << "status"
                                                  << "false"
                                                  << "editable"
                                                  << "false" ) );
    getDBClient().insert( DBNames::FeaturesDB,
                          JSON( db::ID_FIELD_NAME << Test2 << "version"
                                                  << "1.0"
                                                  << "status"
                                                  << "true"
                                                  << "editable"
                                                  << "false" ) );

    FeaturesManager features = FeaturesManager( getDBClient() );
    CoreDRemoteInterface::CoreDRemoteInterface core( getCoreDService() );
    auto featurePtr1 = core.findFeature( Test1 );
    auto featurePtr2 = core.findFeature( Test2 );

    ASSERT_TRUE( featurePtr1.get() == nullptr );
    ASSERT_TRUE( featurePtr2.get() == nullptr );
}

}  // namespace UT
}  // namespace HubPlatform