#include "scenes_manager/tests/fixtures/group_operations_fixture.h"
#include "firmware/core/scenes_manager/scenes_processor/group_operations/device_group_updater.h"
#include "firmware/core/scenes_manager/scenes_processor/group_operations/group_operations_utils.h"
#include "scenes_manager/tests/resources/generic_scene_json.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"

#include <gtest/gtest.h>

namespace HubPlatform {
namespace UT {

/*
TEST PLAN
 1. Device group list with no filters
 2. Device group list with ids filter
 3. Device group list with ids filter error
 4. Device group list with no version
 5. Device group list with version error
*/
static const auto Group1 = "group1";
static const auto Group2 = "group2";
static const auto Group3 = "group3";
static const auto Group4 = "group4";

static const auto GroupName1 = "groupName1";
static const auto GroupName2 = "groupName2";
static const auto GroupName3 = "groupName3";
static const auto GroupName4 = "groupName4";

class DeviceGroupListTest : public GroupOperationsFixture {
public:
    JsonObject devicesGroupsListTestRequestParams( const std::string ids, const std::string version = "" ) {
        std::string request;
        if( !ids.empty() ) {
            request = R"(
        {
            "ids": [__IDS__]
        }
        )";
        }
        else if( !version.empty() ) {
            request = R"(
        {
            "version": __VERSION__
        }
        )";
        }

        KeyStringList keys = { { "__IDS__", ids }, { "__VERSION__", version } };

        request = replaceTokens( request, keys );

        return Json::parse( request );
    }
};

TEST_F( DeviceGroupListTest, listWithNoFilters ) {  // 1
    addDeviceGroup( Group1, GroupName1 );
    addDeviceGroup( Group2, GroupName2 );

    auto answer = getDeviceGroupsList( JSON_EMPTY_OBJ );
    const auto Groups =
        R"({"deviceGroups":[{"_id":"group1","categories":[],"devices":[],"exceptions":[],"name":"groupName1","persistent":false},{"_id":"group2","categories":[],"devices":[],"exceptions":[],"name":"groupName2","persistent":false}],"version":-6305849545365675181})";
    EXPECT_EQ( answer.dump(), Groups );
}

TEST_F( DeviceGroupListTest, listWithFilters ) {  // 2
    addDeviceGroup( Group1, GroupName1 );
    addDeviceGroup( Group2, GroupName2 );
    addDeviceGroup( Group3, GroupName3 );
    addDeviceGroup( Group4, GroupName4 );

    JsonArrayBuilder ids;
    const auto Ids = R"("group2", "group3")";

    auto requestParams = devicesGroupsListTestRequestParams( Ids );
    auto answer = getDeviceGroupsList( requestParams );
    const auto Groups =
        R"({"deviceGroups":[{"_id":"group2","categories":[],"devices":[],"exceptions":[],"name":"groupName2","persistent":false},{"_id":"group3","categories":[],"devices":[],"exceptions":[],"name":"groupName3","persistent":false}],"version":-3397166346308065587})";
    EXPECT_EQ( answer.dump(), Groups );
}

TEST_F( DeviceGroupListTest, listWithFiltersError ) {  // 3
    JsonArrayBuilder ids;
    const auto Ids = "123";
    auto requestParams = devicesGroupsListTestRequestParams( Ids );
    auto answer = getDeviceGroupsList( requestParams );
    auto error = answer[ "error" ];
    EXPECT_EQ( json_get_str( error, "field" ), "ids" );
    EXPECT_EQ( json_get_str( error, "data" ), "rpc.params.invalid" );
    EXPECT_EQ( json_get_str( error, "message" ), "Wrong params" );
}

TEST_F( DeviceGroupListTest, listWithNoVersion ) {  // 4
    addDeviceGroup( Group1, GroupName1 );
    addDeviceGroup( Group2, GroupName2 );
    addDeviceGroup( Group3, GroupName3 );
    const auto Ids = "";
    auto requestParams = devicesGroupsListTestRequestParams( Ids, "123" );
    auto answer = getDeviceGroupsList( requestParams );
    const auto Groups =
        R"({"deviceGroups":[{"_id":"group1","categories":[],"devices":[],"exceptions":[],"name":"groupName1","persistent":false},{"_id":"group2","categories":[],"devices":[],"exceptions":[],"name":"groupName2","persistent":false},{"_id":"group3","categories":[],"devices":[],"exceptions":[],"name":"groupName3","persistent":false}],"version":1612076980504293840})";
    EXPECT_EQ( answer.dump(), Groups );

    requestParams = devicesGroupsListTestRequestParams( Ids, "1612076980504293840" );
    answer = getDeviceGroupsList( requestParams );
    const auto answerVersion = R"({"version":1612076980504293840})";
    EXPECT_EQ( answer.dump(), answerVersion );
}

TEST_F( DeviceGroupListTest, listWithVersionError ) {  // 5
    const auto Ids = "";
    auto requestParams = devicesGroupsListTestRequestParams( Ids, R"("123")" );
    auto answer = getDeviceGroupsList( requestParams );
    const auto ExpectedAnswer =
        R"({"error":{"code":-32602,"data":"rpc.params.invalid","field":"version","message":"Wrong params","reason":"Wrong value type"}})";
    EXPECT_EQ( answer.dump(), ExpectedAnswer );
}

}  // namespace UT
}  // namespace HubPlatform