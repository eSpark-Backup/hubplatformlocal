
#include "include/json.hpp"
#include "tiny/tiny_consolecolors.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>

namespace tiny {
namespace UT {
TEST( JsonTestSuite, fixBrokenConstancyOnConstChildGetting ) {
    nlohmann::basic_json<> json;

    json[ "1" ] = "2";

    const auto& constJson = json;

    EXPECT_FALSE( constJson[ "1" ].is_null() );
    EXPECT_TRUE( constJson[ "2" ].is_null() );

    EXPECT_FALSE( constJson[ std::string( "1" ) ].is_null() );
    EXPECT_TRUE( constJson[ std::string( "2" ) ].is_null() );

    EXPECT_EQ( constJson.find( "2" ), constJson.end() );
    EXPECT_NE( constJson.find( "1" ), constJson.end() );
}

TEST( ConsoleColors, formatIntegers ) {
    nlohmann::basic_json<> json;

    using namespace std::chrono;
    int64_t now = duration_cast< milliseconds >( system_clock::now().time_since_epoch() ).count();
    json[ "now" ] = now;

    auto dump = json.dump();
    auto coloredDump = cc::c( json, -1 );
    ASSERT_EQ( coloredDump, dump ) << "Should not truncate int64_t to int";
}

}  // namespace UT
}  // namespace tiny
