#include <znet/znet_node.hpp>
#include <znet/cc_0x7a_firmware_update.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

namespace znet {
namespace ut {

using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcFirmwareUpdateTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcFirmwareUpdateTestSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    {
        MockFunction<void( RequestFailTypeOpt )> callback;

        node->addCommandClass( 0, ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD, 5 );
        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_EQ( cc->getFirmwareId(), 0 );
        EXPECT_EQ( cc->getManufacturerId(), 0 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 0 );
        EXPECT_FALSE( cc->isFirmwareUpgradable() );
        EXPECT_FALSE( cc->getMaxFragmentSize().has_value() );
        EXPECT_FALSE( cc->getHardwareRevision().has_value() );
        ASSERT_EQ( cc->getFirmwaresCount(), 0 );

        znet_firmware_md_report_v1_t report_v1;
        report_v1.manufacturer_id = 100;
        report_v1.firmware_id = 2000;
        report_v1.checksum = 5555;

        EXPECT_FALSE( cc->hasInterviewDone() );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        node->getRequestsManager().onRespond( FirmwareMdRequest( 1, 0 ), report_v1, nullptr, nullptr );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getFirmwareId(), 2000 );
        EXPECT_EQ( cc->getManufacturerId(), 100 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 5555 );
        EXPECT_TRUE( cc->isFirmwareUpgradable() );
        EXPECT_FALSE( cc->getMaxFragmentSize().has_value() );
        EXPECT_FALSE( cc->getHardwareRevision().has_value() );
        ASSERT_EQ( cc->getFirmwaresCount(), 1 );
        EXPECT_EQ( cc->getFirmwareId( 0 ), 2000 );

        eraseCommandClass( cc );
    }

    {
        MockFunction<void( RequestFailTypeOpt )> callback;

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 0 );

        node->addCommandClass( 0, ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD, 5 );
        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );
        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        znet_firmware_md_report_v1_t report_v1;
        report_v1.manufacturer_id = 1000;
        report_v1.firmware_id = 20000;
        report_v1.checksum = 55555;

        znet_firmware_md_report_v3_t report_v3;
        report_v3.firmware_upgradable = false;
        report_v3.max_fragment_size = 30;
        report_v3.additional_firmware_count = 4;
        uint16_t targetIds[] = { 1, 11, 111, 1111 };
        report_v3.additional_firmware = targetIds;

        EXPECT_FALSE( cc->hasInterviewDone() );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        node->getRequestsManager().onRespond( FirmwareMdRequest( 1, 0 ), report_v1, &report_v3, nullptr );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getFirmwareId(), 20000 );
        EXPECT_EQ( cc->getManufacturerId(), 1000 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 55555 );
        EXPECT_FALSE( cc->isFirmwareUpgradable() );
        EXPECT_EQ( cc->getMaxFragmentSize(), utils::optional<uint16_t>( 30 ) );
        EXPECT_FALSE( cc->getHardwareRevision().has_value() );
        ASSERT_EQ( cc->getFirmwaresCount(), 5 );
        EXPECT_EQ( cc->getFirmwareId( 0 ), 20000 );
        EXPECT_EQ( cc->getFirmwareId( 1 ), 1 );
        EXPECT_EQ( cc->getFirmwareId( 2 ), 11 );
        EXPECT_EQ( cc->getFirmwareId( 3 ), 111 );
        EXPECT_EQ( cc->getFirmwareId( 4 ), 1111 );

        eraseCommandClass( cc );
    }

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD, 5 );
        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );
        EXPECT_TRUE( cc->interview( {}, {} ) );

        znet_firmware_md_report_v1_t report_v1;
        report_v1.manufacturer_id = 2000;
        report_v1.firmware_id = 30000;
        report_v1.checksum = 44444;

        znet_firmware_md_report_v3_t report_v3;
        report_v3.firmware_upgradable = true;
        report_v3.max_fragment_size = 20;
        report_v3.additional_firmware_count = 3;
        uint16_t targetIds[] = { 1, 11, 111 };
        report_v3.additional_firmware = targetIds;

        znet_firmware_md_report_v5_t report_v5;
        report_v5.hardware_revision = 4;

        node->getRequestsManager().onRespond( FirmwareMdRequest( 1, 0 ), report_v1, &report_v3, &report_v5 );

        EXPECT_EQ( cc->getFirmwareId(), 30000 );
        EXPECT_EQ( cc->getManufacturerId(), 2000 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 44444 );
        EXPECT_TRUE( cc->isFirmwareUpgradable() );
        EXPECT_EQ( cc->getMaxFragmentSize(), utils::optional<uint16_t>( 20 ) );
        EXPECT_EQ( cc->getHardwareRevision(), utils::optional<uint8_t>( 4 ) );
        ASSERT_EQ( cc->getFirmwaresCount(), 4 );
        EXPECT_EQ( cc->getFirmwareId( 0 ), 30000 );
        EXPECT_EQ( cc->getFirmwareId( 1 ), 1 );
        EXPECT_EQ( cc->getFirmwareId( 2 ), 11 );
        EXPECT_EQ( cc->getFirmwareId( 3 ), 111 );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcFirmwareUpdateTestSuite, storeRestore ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD, 5 );
        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x01,  // flags
            0x01,  // targets count
            0x09,
            0x00,  // targets
            0x05,
            0x01,  // manufacturer id
            0x03,
            0x02  // checksum
        } );
        EXPECT_TRUE( cc->restoreData( sourceFile, 1 ) );

        EXPECT_EQ( cc->getFirmwareId(), 0x0009 );
        EXPECT_EQ( cc->getManufacturerId(), 0x0105 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 0x0203 );
        EXPECT_TRUE( cc->isFirmwareUpgradable() );
        EXPECT_FALSE( cc->getMaxFragmentSize().has_value() );
        EXPECT_FALSE( cc->getHardwareRevision().has_value() );
        ASSERT_EQ( cc->getFirmwaresCount(), 1 );
        EXPECT_EQ( cc->getFirmwareId( 0 ), 0x0009 );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD, 5 );
        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x02,  // flags
            0x03,  // targets count
            0x08,
            0x00,
            0x11,
            0xAB,
            0x10,
            0x0B,  // targets
            0x05,
            0x02,  // manufacturer id
            0x08,
            0x02,  // checksum
            0x0D,
            0x00  // fragment size
        } );
        EXPECT_TRUE( cc->restoreData( sourceFile, 1 ) );

        EXPECT_EQ( cc->getFirmwareId(), 0x0008 );
        EXPECT_EQ( cc->getManufacturerId(), 0x0205 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 0x0208 );
        EXPECT_FALSE( cc->isFirmwareUpgradable() );
        EXPECT_EQ( cc->getMaxFragmentSize(), utils::optional<uint16_t>( 0x000D ) );
        EXPECT_FALSE( cc->getHardwareRevision().has_value() );
        ASSERT_EQ( cc->getFirmwaresCount(), 3 );
        EXPECT_EQ( cc->getFirmwareId( 0 ), 0x0008 );
        EXPECT_EQ( cc->getFirmwareId( 1 ), 0xAB11 );
        EXPECT_EQ( cc->getFirmwareId( 2 ), 0x0B10 );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }

    {
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD, 5 );
        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );

        FakeFile sourceFile( std::vector<uint8_t>{
            0x07,  // flags
            0x02,  // targets count
            0x08,
            0x00,
            0x11,
            0xAB,  // targets
            0x05,
            0x02,  // manufacturer id
            0x08,
            0x02,  // checksum
            0x0E,
            0x00,  // fragment size,
            0xBC   // hardware version
        } );
        EXPECT_TRUE( cc->restoreData( sourceFile, 1 ) );

        EXPECT_EQ( cc->getFirmwareId(), 0x0008 );
        EXPECT_EQ( cc->getManufacturerId(), 0x0205 );
        EXPECT_EQ( cc->getFirmwareChecksum(), 0x0208 );
        EXPECT_TRUE( cc->isFirmwareUpgradable() );
        EXPECT_EQ( cc->getMaxFragmentSize(), utils::optional<uint16_t>( 0x000E ) );
        EXPECT_EQ( cc->getHardwareRevision(), utils::optional<uint8_t>( 0xBC ) );
        ASSERT_EQ( cc->getFirmwaresCount(), 2 );
        EXPECT_EQ( cc->getFirmwareId( 0 ), 0x0008 );
        EXPECT_EQ( cc->getFirmwareId( 1 ), 0xAB11 );

        FakeFile destFile;
        EXPECT_TRUE( cc->storeData( destFile ) );
        EXPECT_EQ( destFile.getData(), sourceFile.getData() );

        eraseCommandClass( cc );
    }
}

}  // namespace ut
}  // namespace znet
