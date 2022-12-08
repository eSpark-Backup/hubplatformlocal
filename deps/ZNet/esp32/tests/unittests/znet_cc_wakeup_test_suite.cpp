#include <znet/znet_node.hpp>
#include <znet/cc_0x84_wakeup.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcWakeupSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcWakeupSuite, interview_v1 ) {
    auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getMinWakeUpInterval(), 0 );
    EXPECT_EQ( cc->getMaxWakeUpInterval(), 0 );
    EXPECT_EQ( cc->getStepWakeUpInterval(), 0 );
    EXPECT_FALSE( cc->hasCapabilities() );

    EXPECT_EQ( cc->getInterval(), 0 );
}

TEST_F( ZCcWakeupSuite, interview_v2 ) {
    auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 2 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_wakeup_capabilities_report_t capabilities{};
    capabilities.min_wakeup_interval = 10;
    capabilities.max_wakeup_interval = 200000;
    capabilities.default_wakeup_interval = 200;
    capabilities.step_wakeup_interval = 1;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( WakeupCapabilitiesRequest( 1, 0 ), capabilities );

    EXPECT_EQ( cc->getMinWakeUpInterval(), 10 );
    EXPECT_EQ( cc->getMaxWakeUpInterval(), 200000 );
    EXPECT_EQ( cc->getStepWakeUpInterval(), 1 );
    EXPECT_TRUE( cc->hasCapabilities() );
    EXPECT_EQ( cc->getInterval(), 0 );
}

TEST_F( ZCcWakeupSuite, interview_v2_fail ) {
    auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 2 );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;

        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        getRequestsManager().onFail( WakeupCapabilitiesRequest( 1, 0 ) );

        EXPECT_EQ( cc->getMinWakeUpInterval(), 0 );
        EXPECT_EQ( cc->getMaxWakeUpInterval(), 0 );
        EXPECT_EQ( cc->getStepWakeUpInterval(), 0 );
        EXPECT_FALSE( cc->hasCapabilities() );
        EXPECT_EQ( cc->getInterval(), 0 );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;

        getRequestsManager().addFailedRequest( WakeupCapabilitiesRequest( 1, 0 ) );
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );

        getRequestsManager().onFail( WakeupCapabilitiesRequest( 1, 0 ) );

        EXPECT_EQ( cc->getMinWakeUpInterval(), 0 );
        EXPECT_EQ( cc->getMaxWakeUpInterval(), 0 );
        EXPECT_EQ( cc->getStepWakeUpInterval(), 0 );
        EXPECT_FALSE( cc->hasCapabilities() );
        EXPECT_EQ( cc->getInterval(), 0 );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

TEST_F( ZCcWakeupSuite, requestValue ) {
    auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 2 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( WakeupIntervalRequest( 1, 0 ), 100, 10 );
}

TEST_F( ZCcWakeupSuite, setWakeupInterval ) {
    auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 2 );
    {
        // TODO: check fc_send_data params
        cc->setWakeUpInterval( 6000 );
        EXPECT_EQ( cc->getInterval(), 6000 );

        cc->setWakeUpInterval( 0 );
        EXPECT_EQ( cc->getInterval(), 0 );
    }

    cc->interview( {}, {} );

    znet_wakeup_capabilities_report_t capabilities{};
    capabilities.min_wakeup_interval = 10;
    capabilities.max_wakeup_interval = 2000;
    capabilities.default_wakeup_interval = 200;
    capabilities.step_wakeup_interval = 1;
    getRequestsManager().onRespond( WakeupCapabilitiesRequest( 1, 0 ), capabilities );

    // set interval out of range
    {
        // TODO: check fc_send_data params
        cc->setWakeUpInterval( 7000 );
        EXPECT_EQ( cc->getInterval(), 2000 );

        cc->setWakeUpInterval( 5 );
        EXPECT_EQ( cc->getInterval(), 10 );
    }

    // set correct interval
    {
        cc->setWakeUpInterval( 1500 );
        EXPECT_EQ( cc->getInterval(), 1500 );

        cc->setWakeUpInterval( 0 );
        EXPECT_EQ( cc->getInterval(), 0 );
    }

    // set interval without caching immediatelly
    {
        // TODO: check fc_send_data params
        cc->setWakeUpInterval( 1500, false );
        EXPECT_EQ( cc->getInterval(), 0 );
    }
}

TEST_F( ZCcWakeupSuite, storeRestore ) {
    // store/restore for 12 revision, cc version 2
    {
        auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 2 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x01,
            0x02,
            0x03,
            0x04,  // min interval
            0x05,
            0x06,
            0x07,
            0x08,  // max interval
            0x01,
            0x02,
            0x00,
            0x00,  // step
            0x01,  // has capabilities
            0x02,
            0x03,
            0x04,
            0x05  // interval
        } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 12 ) );
        EXPECT_EQ( cc->getMinWakeUpInterval(), 0x04030201 );
        EXPECT_EQ( cc->getMaxWakeUpInterval(), 0x08070605 );
        EXPECT_EQ( cc->getStepWakeUpInterval(), 0x0201 );
        EXPECT_TRUE( cc->hasCapabilities() );
        EXPECT_EQ( cc->getInterval(), 0x05040302 );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    // store/restore for revision 12, version 1
    {
        auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 1 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x00,
            0x00,
            0x00,
            0x00,  // min interval
            0x00,
            0x00,
            0x00,
            0x00,  // max interval
            0x00,
            0x00,
            0x00,
            0x00,  // step
            0x00,  // has capabilities
            0x02,
            0x03,
            0x04,
            0x05  // interval
        } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 12 ) );
        EXPECT_EQ( cc->getMinWakeUpInterval(), 0 );
        EXPECT_EQ( cc->getMaxWakeUpInterval(), 0 );
        EXPECT_EQ( cc->getStepWakeUpInterval(), 0 );
        EXPECT_FALSE( cc->hasCapabilities() );
        EXPECT_EQ( cc->getInterval(), 0x05040302 );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    // data saved in rev 11, version 2
    {
        auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 1 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x01,
            0x02,
            0x03,
            0x04,  // min interval
            0x05,
            0x06,
            0x07,
            0x08,  // max interval
            0x01,
            0x02,
            0x00,
            0x00,  // step
            0x01   // has capabilities
        } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 11 ) );
        EXPECT_EQ( cc->getMinWakeUpInterval(), 0x04030201 );
        EXPECT_EQ( cc->getMaxWakeUpInterval(), 0x08070605 );
        EXPECT_EQ( cc->getStepWakeUpInterval(), 0x0201 );
        EXPECT_TRUE( cc->hasCapabilities() );
        EXPECT_EQ( cc->getInterval(), 0 );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        auto expectedData = sourceFile.getData();
        expectedData.push_back( 0x00 );
        expectedData.push_back( 0x00 );
        expectedData.push_back( 0x00 );
        expectedData.push_back( 0x00 );
        EXPECT_EQ( destFile.getData(), expectedData );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcWakeupSuite, onIntervalReceived ) {
    auto cc = ensureNodeAndCc<ZCc0x84WakeUp>( 1, 0, 2 );

    cc->onIntervalReceived( 100, znet_get_self_node_id() );
    EXPECT_EQ( cc->getInterval(), 100 );

    cc->onIntervalReceived( 100, znet_get_self_node_id() + 1 );
    EXPECT_EQ( cc->getInterval(), 0 );

    cc->onIntervalReceived( 100, znet_get_self_node_id() );
    cc->onIntervalReceived( 0, znet_get_self_node_id() );
    EXPECT_EQ( cc->getInterval(), 0 );
}

}  // namespace ut
}  // namespace znet
