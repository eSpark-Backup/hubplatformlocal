#include <znet/znet_node.hpp>
#include <znet/cc_0x5d_anti_theft.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "MainFixture.h"
#include "znet_fc_send_data_mock.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcAntiTheftTestSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
};

TEST_F( ZCcAntiTheftTestSuite, interview ) {
    // not locked
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );
        MockFunction<void( RequestFailTypeOpt )> callback;

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        znet_cmd_anti_theft_report_t report{};
        const uint8_t hint[] = { 0x02, 0x03, 0x04 };
        report.is_locked = false;
        report.is_restricted = true;
        report.hint = hint;
        report.hint_length = 3;
        report.manufacturer_id = 0x0102;
        report.zwave_locking_id = 0x0304;

        getRequestsManager().onRespond( AntiTheftRequest( 2, 0 ), report );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getInitialLockInfo(), AntiTheftInfoOpt() );

        eraseCommandClass( cc );
    }

    // locked
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );
        MockFunction<void( RequestFailTypeOpt )> callback;

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        znet_cmd_anti_theft_report_t report{};
        const uint8_t hint[] = { 0x02, 0x03, 0x04 };
        report.is_locked = true;
        report.is_restricted = true;
        report.hint = hint;
        report.hint_length = 3;
        report.manufacturer_id = 0x0102;
        report.zwave_locking_id = 0x0304;

        getRequestsManager().onRespond( AntiTheftRequest( 2, 0 ), report );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getInitialLockInfo(),
                   AntiTheftInfoOpt( AntiTheftInfo( true, { 0x02, 0x03, 0x04 }, 0x0102, { 0x0304 } ) ) );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcAntiTheftTestSuite, lock ) {
    // v2
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );

        EXPECT_CALL( *this,
                     fcSendData( 2,
                                 0,
                                 std::vector<uint8_t>{
                                     ZNET_COMMAND_CLASS_ANTITHEFT,
                                     ANTITHEFT_SET,
                                     0x80 | 0x03,  // enabled | code size
                                     0xAE,
                                     0xBC,
                                     0x1D,  // code
                                     0x01,
                                     0x02,  // manufacturer id
                                     0x04,  // hint size
                                     0x1F,
                                     0x03,
                                     0x2C,
                                     0x49,  // hint
                                     0x09,
                                     0x01  // locking id
                                 },
                                 false,
                                 false,
                                 Encapsulation_None ) );

        cc->lock( "AEBC1D", "1F032C49", 0x0102, 0x0901 );

        eraseCommandClass( cc );
    }

    // v3
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 3 );

        EXPECT_CALL( *this,
                     fcSendData( 2,
                                 0,
                                 std::vector<uint8_t>{
                                     ZNET_COMMAND_CLASS_ANTITHEFT,
                                     ANTITHEFT_SET,
                                     0x80 | 0x03,  // enabled | code size
                                     0xAE,
                                     0xBC,
                                     0x1D,  // code
                                     0x01,
                                     0x02,  // manufacturer id
                                     0x04,  // hint size
                                     0x1F,
                                     0x03,
                                     0x2C,
                                     0x49,  // hint
                                     0x03,
                                     0x04  // locking id
                                 },
                                 false,
                                 false,
                                 Encapsulation_None ) );

        cc->lock( "AEBC1D", "1F032C49", 0x0102, 0x0304 );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcAntiTheftTestSuite, unlock ) {
    // v2
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );

        EXPECT_CALL( *this,
                     fcSendData( 2,
                                 0,
                                 std::vector<uint8_t>{
                                     ZNET_COMMAND_CLASS_ANTITHEFT,
                                     ANTITHEFT_SET,
                                     0 | 0x03,  // disabled | code size
                                     0xAE,
                                     0xBC,
                                     0x1D,  // code
                                     0,
                                     0,     // manufacturer id
                                     0x00,  // hint size
                                     0,
                                     0  // locking id
                                 },
                                 false,
                                 false,
                                 Encapsulation_None ) );

        cc->unlock( "AEBC1D" );

        eraseCommandClass( cc );
    }

    // v2
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 3 );

        EXPECT_CALL( *this,
                     fcSendData( 2,
                                 0,
                                 std::vector<uint8_t>{
                                     ZNET_COMMAND_CLASS_ANTITHEFT,
                                     ANTITHEFT_SET,
                                     0 | 0x03,  // disabled | code size
                                     0xAE,
                                     0xBC,
                                     0x1D,  // code
                                     0,
                                     0,     // manufacturer id
                                     0x00,  // hint size
                                     0,
                                     0  // locking id
                                 },
                                 false,
                                 false,
                                 Encapsulation_None ) );

        cc->unlock( "AEBC1D" );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcAntiTheftTestSuite, storeRestore ) {
    // not locked
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );

        FakeFile sourceFile( std::vector<uint8_t>{ 0x00 } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 1 ) );
        EXPECT_EQ( sourceFile.getReadPos(), sourceFile.getData().size() );

        EXPECT_FALSE( cc->getInitialLockInfo().has_value() );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    // locked, no locking id
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x01,  // has value
            0x01,  // is restricted
            0x04,  // hint size
            0x01,
            0x02,
            0x03,
            0x04,  // hint
            0x06,
            0x05,  // manufacturer id
            0x00   // no locking id
        } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 1 ) );
        EXPECT_EQ( sourceFile.getReadPos(), sourceFile.getData().size() );

        EXPECT_EQ( cc->getInitialLockInfo(),
                   AntiTheftInfoOpt( AntiTheftInfo( true, { 0x01, 0x02, 0x03, 0x04 }, 0x0506, {} ) ) );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    // locked, has locking id
    {
        auto cc = ensureNodeAndCc<ZCc0x5DAntiTheft>( 2, 0, 2 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x01,  // has value
            0x00,  // is restricted
            0x03,  // hint size
            0x02,
            0x03,
            0x04,  // hint
            0x06,
            0x05,  // manufacturer id
            0x01,  // has locking id
            0x08,
            0x07  // no locking id
        } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 1 ) );
        EXPECT_EQ( sourceFile.getReadPos(), sourceFile.getData().size() );

        EXPECT_EQ( cc->getInitialLockInfo(),
                   AntiTheftInfoOpt( AntiTheftInfo( false, { 0x02, 0x03, 0x04 }, 0x0506, { 0x0708 } ) ) );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }
}

}  // namespace ut
}  // namespace znet
