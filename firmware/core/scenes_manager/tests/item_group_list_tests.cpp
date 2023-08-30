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
 1. Item group list with no filters
 2. Item group list with ids filter
 3. Item group list with ids filter error
 4. Item group list with no version
 5. Item group list with version error
*/

static const auto Group1 = "group1";
static const auto Group2 = "group2";
static const auto Group3 = "group3";
static const auto Group4 = "group4";

static const auto GroupName1 = "groupName1";
static const auto GroupName2 = "groupName2";
static const auto GroupName3 = "groupName3";
static const auto GroupName4 = "groupName4";

class ItemGroupListTest : public GroupOperationsFixture {
public:
    JsonObject itemsGroupsListTestRequestParams( const std::string ids, const std::string version = "" ) {
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

TEST_F( ItemGroupListTest, listWithNoFilters ) {  // 1
    addItemGroup( Group1, GroupName1 );

    auto answer = getItemGroupsList( JSON_EMPTY_OBJ );
    const auto Groups =
        R"({"itemGroups":[{"_id":"group1","enum":[],"hasGetter":false,"hasSetter":false,"itemNames":[],"name":"groupName1","persistent":false,"valueType":"","valueTypeFamily":""}],"version":-3191078521215324708})";
    EXPECT_EQ( answer.dump(), Groups );
}

TEST_F( ItemGroupListTest, listWithFilters ) {  // 2
    addItemGroup( Group1, GroupName1 );
    addItemGroup( Group2, GroupName2 );
    addItemGroup( Group3, GroupName3 );
    addItemGroup( Group4, GroupName4 );

    JsonArrayBuilder ids;
    const auto Ids = R"("group2", "group3")";

    auto requestParams = itemsGroupsListTestRequestParams( Ids );
    auto answer = getItemGroupsList( requestParams );
    const auto Groups =
        R"({"itemGroups":[{"_id":"group2","enum":[],"hasGetter":false,"hasSetter":false,"itemNames":[],"name":"groupName2","persistent":false,"valueType":"","valueTypeFamily":""},{"_id":"group3","enum":[],"hasGetter":false,"hasSetter":false,"itemNames":[],"name":"groupName3","persistent":false,"valueType":"","valueTypeFamily":""}],"version":4027637281372897336})";
    EXPECT_EQ( answer.dump(), Groups );
}

TEST_F( ItemGroupListTest, listWithFiltersError ) {  // 3
    JsonArrayBuilder ids;
    const auto Ids = "123";
    auto requestParams = itemsGroupsListTestRequestParams( Ids );
    auto answer = getItemGroupsList( requestParams );
    auto error = answer[ "error" ];
    EXPECT_EQ( json_get_str( error, "field" ), "ids" );
    EXPECT_EQ( json_get_str( error, "data" ), "rpc.params.invalid" );
    EXPECT_EQ( json_get_str( error, "message" ), "Wrong params" );
}

TEST_F( ItemGroupListTest, listWithNoVersion ) {  // 4
    addItemGroup( Group1, GroupName1 );
    addItemGroup( Group2, GroupName2 );
    addItemGroup( Group3, GroupName3 );
    const auto Ids = "";
    auto requestParams = itemsGroupsListTestRequestParams( Ids, "123" );
    auto answer = getItemGroupsList( requestParams );
    const auto Groups =
        R"({"itemGroups":[{"_id":"group1","enum":[],"hasGetter":false,"hasSetter":false,"itemNames":[],"name":"groupName1","persistent":false,"valueType":"","valueTypeFamily":""},{"_id":"group2","enum":[],"hasGetter":false,"hasSetter":false,"itemNames":[],"name":"groupName2","persistent":false,"valueType":"","valueTypeFamily":""},{"_id":"group3","enum":[],"hasGetter":false,"hasSetter":false,"itemNames":[],"name":"groupName3","persistent":false,"valueType":"","valueTypeFamily":""}],"version":5771246957515657845})";
    EXPECT_EQ( answer.dump(), Groups );

    requestParams = itemsGroupsListTestRequestParams( Ids, "6142509188972423790" );
    answer = getDeviceGroupsList( requestParams );
    const auto answerVersion = R"({"version":6142509188972423790})";
    EXPECT_EQ( answer.dump(), answerVersion );
}

TEST_F( ItemGroupListTest, listWithVersionError ) {  // 5
    const auto Ids = "";
    auto requestParams = itemsGroupsListTestRequestParams( Ids, R"("123")" );
    auto answer = getDeviceGroupsList( requestParams );
    const auto ExpectedAnswer =
        R"({"error":{"code":-32602,"data":"rpc.params.invalid","field":"version","message":"Wrong params","reason":"Wrong value type"}})";
    EXPECT_EQ( answer.dump(), ExpectedAnswer );
}

}  // namespace UT
}  // namespace HubPlatform