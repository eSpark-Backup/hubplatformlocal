#include <znet/znet_node.hpp>
#include <znet/cc_0x7e_anti_theft_unlock.hpp>

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

class ZCcAntiTheftUnlockTestSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
};

TEST_F( ZCcAntiTheftUnlockTestSuite, interview ) {
    // not locked
    {
        auto cc = ensureNodeAndCc<ZCc0x7EAntiTheftUnlock>( 2, 0, 2 );
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

        getRequestsManager().onRespond( AntiTheftUnlockStateRequest( 2, 0 ), report );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getInitialLockInfo(), AntiTheftInfoOpt() );

        eraseCommandClass( cc );
    }

    // locked
    {
        auto cc = ensureNodeAndCc<ZCc0x7EAntiTheftUnlock>( 2, 0, 2 );
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

        getRequestsManager().onRespond( AntiTheftUnlockStateRequest( 2, 0 ), report );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getInitialLockInfo(),
                   AntiTheftInfoOpt( AntiTheftInfo( true, { 0x02, 0x03, 0x04 }, 0x0102, { 0x0304 } ) ) );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcAntiTheftUnlockTestSuite, unlock ) {
    auto cc = ensureNodeAndCc<ZCc0x7EAntiTheftUnlock>( 2, 0, 3 );

    EXPECT_CALL( *this,
                 fcSendData( 2,
                             0,
                             std::vector<uint8_t>{
                                 ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK,
                                 ANTITHEFT_UNLOCK_SET,
                                 0x03,  // code size
                                 0x0E,
                                 0xBC,
                                 0x1D  // code
                             },
                             false,
                             false,
                             Encapsulation_None ) );

    cc->unlock( "0EBC1D" );

    eraseCommandClass( cc );
}

}  // namespace ut
}  // namespace znet
