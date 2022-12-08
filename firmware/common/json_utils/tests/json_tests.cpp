#include "json_utils/json_utils.h"
#include <gtest/gtest.h>

namespace HubPlatform {
namespace UT {

class JsonTests : public ::testing::Test {};

TEST_F( JsonTests, jsonMapOfStrings ) {
    using Value = std::string;
    using TheMap = std::map< std::string, Value >;
    TheMap source = { { "a", "b" }, { "c", "d" } };
    Json json;
    json[ "headers" ] = source;

    TheMap result = json_get_map< Value >( json, "headers" );
    ASSERT_EQ( result.size(), source.size() );
    ASSERT_EQ( result.at( "a" ), source.at( "a" ) );
    ASSERT_EQ( result.at( "c" ), source.at( "c" ) );
}

TEST_F( JsonTests, jsonMapOfInts ) {
    Json json;
    using Value = int;
    using TheMap = std::map< std::string, Value >;
    TheMap source = { { "a", 1 }, { "c", 3 } };
    json[ "headers" ] = source;

    TheMap result = json_get_map< Value >( json, "headers" );
    ASSERT_EQ( result.size(), source.size() );
    ASSERT_EQ( result.at( "a" ), 1 );
    ASSERT_EQ( result.at( "c" ), 3 );
}

TEST_F( JsonTests, jsonMultimapOfStrings ) {
    using Value = std::string;
    using TheMap = std::multimap< std::string, Value >;
    Json json;
    Json sourceArray = JSON_ARRAY_( { { "key", "a" }, { "value", "valForA" } },
                                    { { "key", "a" }, { "value", "secondValForA" } },
                                    { { "key", "b" }, { "value", "valForB" } } );

    json[ "headers" ] = sourceArray;

    TheMap result = json_get_multimap< Value >( sourceArray, "key", "value" );
    ASSERT_EQ( result.size(), sourceArray.size() );

    auto range = result.equal_range( "a" );
    ASSERT_TRUE( range.first != range.second ) << "found range for key 'a' should not be empty";
    ASSERT_EQ( std::distance( range.first, range.second ), 2 ) << "two elements for a key 'a' expected";

    range = result.equal_range( "b" );
    ASSERT_TRUE( range.first != range.second ) << "found range for key 'b' should not be empty";
    ASSERT_EQ( std::distance( range.first, range.second ), 1 ) << "one elements for a key 'b' expected";
}

TEST_F( JsonTests, jsonObjectKeys ) {
    {
        Json object = Json::object( { { "a", "v1" }, { "c", "v2" } } );

        const auto& keysArray = json_object_keys( object );
        ASSERT_EQ( keysArray.size(), 2 );
        ASSERT_EQ( keysArray.at( 0 ), "a" );
        ASSERT_EQ( keysArray.at( 1 ), "c" );
    }

    {
        Json object = Json::object();

        const auto& keysArray = json_object_keys( object );
        ASSERT_EQ( keysArray.size(), 0 );
    }
}

}  // namespace UT
}  // namespace HubPlatform