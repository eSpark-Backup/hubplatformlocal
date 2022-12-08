#include "gtest/gtest.h"

#include <znet/znet.h>
#include <znet/converters/user_code.hpp>
#include <znet/enums/user_code.hpp>

namespace znet {

TEST( UserCodesTestSuite, calculate_user_codes_checksum ) {
    // empty user codes
    { EXPECT_EQ( calculate_user_codes_checksum( NULL, 0 ), 0x1D0F ); }

    auto makeUserCode = []( znet_supported_users_t id, uint8_t status, const char* code ) {
        return znet_user_code_report_t{ .user_id = id,
                                        .user_id_status = status,
                                        .user_code_size = static_cast<uint8_t>( strlen( code ) ),
                                        .user_code = reinterpret_cast<const uint8_t*>( code ) };
    };

    // Single user code
    {
        const auto userCode1 = makeUserCode( 7, 3, "12345" );  // 0x00, 0x07, 0x03, 0x31, 0x32, 0x33, 0x34, 0x35
        EXPECT_EQ( calculate_user_codes_checksum( &userCode1, 1 ), 0x62F1 );

        const auto userCode2 = makeUserCode( 0x100, 0xab, "" );  // 0x01, 0x00, 0xab
        EXPECT_EQ( calculate_user_codes_checksum( &userCode2, 1 ), 0x22BD );
    }

    // A few user codes
    {
        const znet_user_code_report_t userCodesUnordered[3] = {
            makeUserCode( 7, 3, "" ),
            makeUserCode( 0x201, 4, "1234" ),
            makeUserCode( 0x102, 5, "5678" ),
        };
        // 0x00, 0x07, 0x03, 0x01, 0x02, 0x05, 0x35, 0x36, 0x37, 0x38, 0x02, 0x01, 0x04, 0x31, 0x32, 0x33, 0x34

        EXPECT_EQ( calculate_user_codes_checksum( userCodesUnordered, 3 ), 0x9D3 );
    }
}

TEST( UserCodesTestSuite, userModeConverter ) {
    EXPECT_EQ( user_code_converter::userIdStatusToString( UserCode::UserIDStatus::Enabled ), std::string( "enabled" ) );
    EXPECT_EQ( user_code_converter::userIdStatusToString( UserCode::UserIDStatus::PassageMode ),
               std::string( "passage" ) );

    EXPECT_EQ( user_code_converter::userIdStatusFromString( "available" ),
               user_code_converter::UserIdStatusOpt{ UserCode::UserIDStatus::Available } );
    EXPECT_EQ( user_code_converter::userIdStatusFromString( "enabled" ),
               user_code_converter::UserIdStatusOpt{ UserCode::UserIDStatus::Enabled } );
    EXPECT_EQ( user_code_converter::userIdStatusFromString( "messaging" ),
               user_code_converter::UserIdStatusOpt{ UserCode::UserIDStatus::Messaging } );
    EXPECT_FALSE( user_code_converter::userIdStatusFromString( "unexisting mode" ).has_value() );
}

}  // namespace znet
