#include <znet/znet_node.hpp>
#include <znet/cc_0x62_door_lock.hpp>

#include <gtest/gtest.h>

#include "MainFixture.h"
#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "znet_fc_send_data_mock.h"

namespace znet {

static bool operator==( const DoorCondition& a, const DoorCondition& b ) {
    return a.latchState == b.latchState && a.boltState == b.boltState && a.doorState == b.doorState;
}

static bool operator==( const ZCc0x62DoorLock::Capabilities& a, const ZCc0x62DoorLock::Capabilities& b ) {
    return a.supportedInsideDoorHandlesBitmask == b.supportedInsideDoorHandlesBitmask &&
           a.supportedOutsideDoorHandlesBitmask == b.supportedOutsideDoorHandlesBitmask &&
           a.isBoltSupported == b.isBoltSupported && a.isDoorSupported == b.isDoorSupported &&
           a.isLatchSupported == b.isLatchSupported && a.isBlockToBlockSupported == b.isBlockToBlockSupported &&
           a.isTwistAssistSupported == b.isTwistAssistSupported &&
           a.isHoldAndReleaseSupported == b.isHoldAndReleaseSupported &&
           a.isAutoRelockSupported == b.isAutoRelockSupported &&
           a.areTimeOperationsSupported == b.areTimeOperationsSupported && a.supportedModes == b.supportedModes;
}

namespace ut {

class ZCcDoorLockTestSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
};

TEST_F( ZCcDoorLockTestSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 3 );
        auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );
        EXPECT_FALSE( cc->interview( {}, {} ) );
        EXPECT_EQ( cc->getCapabilities(), nullptr );

        eraseCommandClass( cc );
    }

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
        auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );
        ASSERT_NE( cc->getCapabilities(), nullptr );
        EXPECT_EQ( *cc->getCapabilities(), ZCc0x62DoorLock::Capabilities{} );
        EXPECT_TRUE( cc->interview( {}, {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );

        ZCc0x62DoorLock::Capabilities capabilities;

        capabilities.supportedInsideDoorHandlesBitmask = 0x7;
        capabilities.supportedOutsideDoorHandlesBitmask = 0x1;
        capabilities.isDoorSupported = true;
        capabilities.isBoltSupported = true;
        capabilities.isLatchSupported = false;
        capabilities.isBlockToBlockSupported = true;
        capabilities.isTwistAssistSupported = false;
        capabilities.isHoldAndReleaseSupported = true;
        capabilities.areTimeOperationsSupported = true;
        capabilities.isAutoRelockSupported = false;
        capabilities.supportedModes = {
            DoorLock::Mode::Secured, DoorLock::Mode::Unsecured, DoorLock::Mode::Unsecured_WithTimeout
        };

        znet_door_lock_capabilities_t report{};
        report.supported_inside_handle_ids = 0x7;
        report.supported_outside_handle_ids = 0x1;
        report.is_door_supported = true;
        report.is_bolt_supported = true;
        report.is_latch_supported = false;
        report.is_block_to_block_supported = true;
        report.is_twist_assist_supported = false;
        report.is_hold_and_release_supported = true;
        report.are_timed_operations_supported = true;
        report.is_auto_relock_supported = false;
        const uint8_t modes[] = { 0xFF, 0x00, 0x01 };
        report.supported_door_lock_modes_count = 3;
        report.supported_door_lock_modes = modes;

        getRequestsManager().onRequestSent( DoorLockCapabilitiesRequest( 1, 0 ) );
        getRequestsManager().onRespond( DoorLockCapabilitiesRequest( 1, 0 ), report );

        EXPECT_EQ( *cc->getCapabilities(), capabilities );
        EXPECT_TRUE( cc->hasInterviewDone() );

        eraseCommandClass( cc );
    }

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
        auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );
        ASSERT_NE( cc->getCapabilities(), nullptr );
        EXPECT_EQ( *cc->getCapabilities(), ZCc0x62DoorLock::Capabilities{} );
        EXPECT_TRUE( cc->interview( {}, {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );

        ZCc0x62DoorLock::Capabilities capabilities;

        capabilities.supportedInsideDoorHandlesBitmask = 0x5;
        capabilities.supportedOutsideDoorHandlesBitmask = 0x2;
        capabilities.isDoorSupported = false;
        capabilities.isBoltSupported = false;
        capabilities.isLatchSupported = true;
        capabilities.isBlockToBlockSupported = false;
        capabilities.isTwistAssistSupported = true;
        capabilities.isHoldAndReleaseSupported = false;
        capabilities.areTimeOperationsSupported = false;
        capabilities.isAutoRelockSupported = true;
        capabilities.supportedModes = {
            DoorLock::Mode::Secured, DoorLock::Mode::Unsecured, DoorLock::Mode::Unsecured_WithTimeout
        };

        znet_door_lock_capabilities_t report{};
        report.supported_inside_handle_ids = 0x5;
        report.supported_outside_handle_ids = 0x2;
        report.is_door_supported = false;
        report.is_bolt_supported = false;
        report.is_latch_supported = true;
        report.is_block_to_block_supported = false;
        report.is_twist_assist_supported = true;
        report.is_hold_and_release_supported = false;
        report.are_timed_operations_supported = false;
        report.is_auto_relock_supported = true;
        const uint8_t modes[] = { 0xFF, 0x00, 0x01 };
        report.supported_door_lock_modes_count = 3;
        report.supported_door_lock_modes = modes;

        getRequestsManager().onRequestSent( DoorLockCapabilitiesRequest( 1, 0 ) );
        getRequestsManager().onRespond( DoorLockCapabilitiesRequest( 1, 0 ), report );

        EXPECT_EQ( *cc->getCapabilities(), capabilities );
        EXPECT_TRUE( cc->hasInterviewDone() );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcDoorLockTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 2 );
    auto node = getNodesProvider().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
    auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );

    // successful request
    {
        RequestValue requestValue;
        size_t callbackCounter = 0;
        requestValue.resultCallback = [&]( RequestFailTypeOpt failTypeOpt ) {
            EXPECT_FALSE( failTypeOpt );
            ++callbackCounter;
        };
        cc->requestValue( requestValue );

        getRequestsManager().onRequestSent( DoorLockConfigurationRequest( 2, 0 ) );
        getRequestsManager().onRespond(
            DoorLockConfigurationRequest( 2, 0 ), znet_door_lock_configuration_t{}, nullptr );

        EXPECT_EQ( callbackCounter, 0 );

        getRequestsManager().onRequestSent( DoorLockOperationRequest( 2, 0 ) );
        getRequestsManager().onRespond( DoorLockOperationRequest( 2, 0 ), znet_door_lock_operation_report_t{} );

        EXPECT_EQ( callbackCounter, 1 );
    }

    // configuration failed
    {
        RequestValue requestValue;
        size_t callbackCounter = 0;
        requestValue.resultCallback = [&]( RequestFailTypeOpt failTypeOpt ) {
            ASSERT_TRUE( failTypeOpt );
            EXPECT_EQ( *failTypeOpt, RequestFailType::ReportedFail );
            ++callbackCounter;
        };
        cc->requestValue( requestValue );

        getRequestsManager().onRequestSent( DoorLockConfigurationRequest( 2, 0 ) );
        getRequestsManager().onFail( DoorLockConfigurationRequest( 2, 0 ) );

        EXPECT_EQ( callbackCounter, 1 );
    }

    // operation failed
    {
        RequestValue requestValue;
        size_t callbackCounter = 0;
        requestValue.resultCallback = [&]( RequestFailTypeOpt failTypeOpt ) {
            ASSERT_TRUE( failTypeOpt );
            EXPECT_EQ( *failTypeOpt, RequestFailType::ReportedFail );
            ++callbackCounter;
        };
        cc->requestValue( requestValue );

        getRequestsManager().onRequestSent( DoorLockConfigurationRequest( 2, 0 ) );
        getRequestsManager().onRespond(
            DoorLockConfigurationRequest( 2, 0 ), znet_door_lock_configuration_t{}, nullptr );

        getRequestsManager().onRequestSent( DoorLockOperationRequest( 2, 0 ) );
        getRequestsManager().onFail( DoorLockOperationRequest( 2, 0 ) );

        EXPECT_EQ( callbackCounter, 1 );
    }
}

TEST_F( ZCcDoorLockTestSuite, storeRestore ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    ZCc0x62DoorLock::Capabilities capabilities{};

    capabilities.supportedInsideDoorHandlesBitmask = 0x1;
    capabilities.supportedOutsideDoorHandlesBitmask = 0x3;
    capabilities.isDoorSupported = false;
    capabilities.isBoltSupported = true;
    capabilities.isLatchSupported = false;
    capabilities.isBlockToBlockSupported = true;
    capabilities.isTwistAssistSupported = false;
    capabilities.isHoldAndReleaseSupported = true;
    capabilities.areTimeOperationsSupported = false;
    capabilities.isAutoRelockSupported = false;
    capabilities.supportedModes = { DoorLock::Mode::Secured,
                                    DoorLock::Mode::Unsecured,
                                    DoorLock::Mode::Unsecured_WithTimeout,
                                    DoorLock::Mode::Unknown };

    // store/restore for 7 revision, cc version 4
    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
        auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );

        FakeFile sourceFile( std::vector<uint8_t>{ 0x31, 0x2a, 0x04, 0xFF, 0x00, 0x01, 0xFE } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 7 ) );
        ASSERT_NE( cc->getCapabilities(), nullptr );
        EXPECT_EQ( *cc->getCapabilities(), capabilities );

        FakeFile destFile;

        EXPECT_TRUE( cc->storeData( destFile ) );

        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    // restore for 6 revision, cc version 4
    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 4 );
        auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );

        FakeFile sourceFile( std::vector<uint8_t>{ 0x31, 0x2a, 0x04, 0xFF, 0x00, 0x01, 0xFE } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 6 ) );
        ASSERT_NE( cc->getCapabilities(), nullptr );
        EXPECT_EQ( *cc->getCapabilities(), ZCc0x62DoorLock::Capabilities{} );

        eraseCommandClass( cc );
    }

    // restore/store for 7 revision, cc version 3
    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 3 );
        auto cc = node->getCommandClass<ZCc0x62DoorLock>( 0 );

        FakeFile sourceFile( std::vector<uint8_t>{ 0x31, 0x2a, 0x04, 0xFF, 0x00, 0x01, 0xFE } );

        EXPECT_TRUE( cc->restoreData( sourceFile, 7 ) );
        EXPECT_EQ( cc->getCapabilities(), nullptr );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcDoorLockTestSuite, decodeDoorCondition ) {
    EXPECT_EQ(
        ZCc0x62DoorLock::decodeDoorCondition( 0x5 ),
        ( DoorCondition{ DoorLock::LatchState::Closed, DoorLock::BoltState::Locked, DoorLock::DoorState::Closed } ) );

    EXPECT_EQ(
        ZCc0x62DoorLock::decodeDoorCondition( 0x2 ),
        ( DoorCondition{ DoorLock::LatchState::Open, DoorLock::BoltState::Unlocked, DoorLock::DoorState::Open } ) );

    EXPECT_EQ(
        ZCc0x62DoorLock::decodeDoorCondition( 0x1 ),
        ( DoorCondition{ DoorLock::LatchState::Open, DoorLock::BoltState::Locked, DoorLock::DoorState::Closed } ) );

    EXPECT_EQ(
        ZCc0x62DoorLock::decodeDoorCondition( 0xFF ),
        ( DoorCondition{ DoorLock::LatchState::Closed, DoorLock::BoltState::Unlocked, DoorLock::DoorState::Closed } ) );
}

TEST_F( ZCcDoorLockTestSuite, setConfiguration ) {
    auto cc = ensureNodeAndCc<ZCc0x62DoorLock>( 3, 0, 3 );

    {
        EXPECT_CALL(
            *this,
            fcSendData(
                3,
                0,
                std::vector<uint8_t>{
                    ZNET_COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CONFIGURATION_SET, 0x02, 0xA5, 0x21, 0x17, 0, 0, 0, 0, 0 },
                false,
                false,
                Encapsulation_None ) );

        znet_door_lock_configuration_t config{};
        config.lock_timeout = 0x17 + 60 * 0x21;
        config.are_timed_operations_enabled = true;
        config.enabled_inside_handle_ids = 0x05;
        config.enabled_outside_handle_ids = 0xA;

        EXPECT_TRUE( cc->setConfiguration( config, nullptr ) );
    }

    {
        EXPECT_CALL(
            *this,
            fcSendData(
                3,
                0,
                std::vector<uint8_t>{
                    ZNET_COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CONFIGURATION_SET, 0x01, 0xC6, 0xFE, 0xFE, 0, 0, 0, 0, 0 },
                false,
                false,
                Encapsulation_None ) );

        znet_door_lock_configuration_t config{};
        config.lock_timeout = 0x18 + 60 * 0x24;
        config.are_timed_operations_enabled = false;
        config.enabled_inside_handle_ids = 0x06;
        config.enabled_outside_handle_ids = 0x0C;

        znet_door_lock_extended_configuration_t extended_config{};
        extended_config.twist_assist_enabled = true;
        extended_config.auto_relock_timeout = 10;
        extended_config.block_to_block_enabled = true;
        extended_config.hold_and_release_timeout = 30;

        EXPECT_TRUE( cc->setConfiguration( config, &extended_config ) );
    }
}

TEST_F( ZCcDoorLockTestSuite, setConfiguration_v4 ) {
    auto cc = ensureNodeAndCc<ZCc0x62DoorLock>( 3, 0, 4 );

    {
        EXPECT_CALL(
            *this,
            fcSendData(
                3,
                0,
                std::vector<uint8_t>{
                    ZNET_COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CONFIGURATION_SET, 0x02, 0xA5, 0x21, 0x17, 0, 0, 0, 0, 0 },
                false,
                false,
                Encapsulation_None ) );

        znet_door_lock_configuration_t config{};
        config.lock_timeout = 0x17 + 60 * 0x21;
        config.are_timed_operations_enabled = true;
        config.enabled_inside_handle_ids = 0x05;
        config.enabled_outside_handle_ids = 0xA;

        EXPECT_TRUE( cc->setConfiguration( config, nullptr ) );
    }

    {
        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_DOOR_LOCK,
                                                       DOOR_LOCK_CONFIGURATION_SET,
                                                       0x01,
                                                       0xC6,
                                                       0xFE,
                                                       0xFE,
                                                       0x10,
                                                       0x51,
                                                       0xA9,
                                                       0x0C,
                                                       0x03 },
                                 false,
                                 false,
                                 Encapsulation_None ) );

        znet_door_lock_configuration_t config{};
        config.lock_timeout = 0x18 + 60 * 0x24;
        config.are_timed_operations_enabled = false;
        config.enabled_inside_handle_ids = 0x06;
        config.enabled_outside_handle_ids = 0x0C;

        znet_door_lock_extended_configuration_t extended_config{};
        extended_config.twist_assist_enabled = true;
        extended_config.auto_relock_timeout = 0x1051;
        extended_config.block_to_block_enabled = true;
        extended_config.hold_and_release_timeout = 0xA90C;

        EXPECT_TRUE( cc->setConfiguration( config, &extended_config ) );
    }

    {
        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_DOOR_LOCK,
                                                       DOOR_LOCK_CONFIGURATION_SET,
                                                       0x02,
                                                       0xC6,
                                                       0x27,
                                                       0x19,
                                                       0x10,
                                                       0x51,
                                                       0xA9,
                                                       0x0C,
                                                       0x02 },
                                 false,
                                 false,
                                 Encapsulation_None ) );

        znet_door_lock_configuration_t config{};
        config.lock_timeout = 0x19 + 60 * 0x27;
        config.are_timed_operations_enabled = true;
        config.enabled_inside_handle_ids = 0x06;
        config.enabled_outside_handle_ids = 0x0C;

        znet_door_lock_extended_configuration_t extended_config{};
        extended_config.twist_assist_enabled = false;
        extended_config.auto_relock_timeout = 0x1051;
        extended_config.block_to_block_enabled = true;
        extended_config.hold_and_release_timeout = 0xA90C;

        EXPECT_TRUE( cc->setConfiguration( config, &extended_config ) );
    }
}

}  // namespace ut
}  // namespace znet
