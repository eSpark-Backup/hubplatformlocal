#include <znet/znet_node.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <znet/znet_utils.hpp>

#include "znet_node_requests_managing_fixture.h"
#include "znet_fc_send_data_mock.h"
#include "MainFixture.h"
#include "fake_owner_fixture.h"

namespace znet {
namespace ut {

using ::testing::_;
using ::testing::Invoke;

class SupportingFunctionalityTestSuite : public MainFixture, public FakeOwnerFixture, public FcSendDataMock {
protected:
    void SetUp() override {
        MainFixture::SetUp();

        propagateCallback<SupportingFunctionalityTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_firmware_update_info_fill ),
                          &znet_callbacks_t::node_cmd_firmware_update_info_fill>();

        propagateCallback<SupportingFunctionalityTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_manufacturer_specific_fill ),
                          &znet_callbacks_t::node_cmd_manufacturer_specific_fill>();

        propagateCallback<SupportingFunctionalityTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_zwaveplus_info_fill ),
                          &znet_callbacks_t::node_cmd_zwaveplus_info_fill>();
    }
};

TEST_F( SupportingFunctionalityTestSuite, FIRMWARE_MD_GET_test ) {
    ON_CALL( getOwner(), fillFirmwareUpdateInfo( _, _, _ ) )
        .WillByDefault( Invoke( []( znet_firmware_md_report_v1_t& dataV1,
                                    znet_firmware_md_report_v3_t& dataV3,
                                    znet_firmware_md_report_v5_t& dataV5 ) {
            dataV1.manufacturer_id = 0x0102;
            dataV1.firmware_id = 0x0304;
            dataV1.checksum = 0x0506;

            dataV3.max_fragment_size = 0x0809;
            dataV3.firmware_upgradable = true;
            dataV3.additional_firmware_count = 2;
            dataV3.additional_firmware[0] = 0xA0B0;
            dataV3.additional_firmware[1] = 0xC0D0;

            dataV5.hardware_revision = 0xE0;
        } ) );

    const std::vector<uint8_t> expectedReport = {
        COMMAND_CLASS_FIRMWARE_UPDATE_MD,
        FIRMWARE_MD_REPORT,
        0x01,
        0x02,  // numufacturer id
        0x03,
        0x04,  // firmware id
        0x05,
        0x06,  // checksum
        0xFF,  // firmware upgradable
        0x02,  // firmware targets
        0x08,
        0x09,  // max fragment size
        0xA0,
        0xB0,  // firmware id 1
        0xC0,
        0xD0,  // firmware id 2
        0xE0   // hardware version
    };

    EXPECT_CALL( *this, fcSendData( 3, 1, expectedReport, false, false, Encapsulation_Sec_AUTHENTICATED ) );

    onMessageReceived( 3, 1, { COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_MD_GET }, Encapsulation_Sec_AUTHENTICATED );
}

TEST_F( SupportingFunctionalityTestSuite, FIRMWARE_UPDATE_MD_REQUEST_GET_test ) {
    const std::vector<uint8_t> expectedReport = {
        COMMAND_CLASS_FIRMWARE_UPDATE_MD,
        FIRMWARE_UPDATE_MD_REQUEST_REPORT,
        0x03  // status, not downloadable
    };

    EXPECT_CALL( *this, fcSendData( 3, 1, expectedReport, false, false, Encapsulation_Sec_2 ) );

    onMessageReceived(
        3, 1, { COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_UPDATE_MD_REQUEST_GET }, Encapsulation_Sec_2 );
}

TEST_F( SupportingFunctionalityTestSuite, FIRMWARE_UPDATE_MD_PREPARE_GET_test ) {
    const std::vector<uint8_t> expectedReport = {
        COMMAND_CLASS_FIRMWARE_UPDATE_MD,
        FIRMWARE_UPDATE_MD_PREPARE_REPORT,
        0x03,  // status, not upgradable
        0x1D,
        0x0F  // checksum ( initial value )
    };

    EXPECT_CALL( *this, fcSendData( 3, 2, expectedReport, false, false, Encapsulation_Sec_UNAUTHENTICATED ) );

    onMessageReceived(
        3, 2, { COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_UPDATE_MD_PREPARE_GET }, Encapsulation_Sec_UNAUTHENTICATED );
}

TEST_F( SupportingFunctionalityTestSuite, MANUFACTURER_SPECIFIC_GET_test ) {
    ON_CALL( getOwner(), fillManufacturerSpecific( _ ) )
        .WillByDefault( Invoke( []( znet_manufacturer_specific_report_t& report ) {
            report.product_id = 0x0102;
            report.product_type = 0x0304;
            report.manufacturer_id = 0x0506;
        } ) );

    const std::vector<uint8_t> expectedReport = {
        COMMAND_CLASS_MANUFACTURER_SPECIFIC,
        MANUFACTURER_SPECIFIC_REPORT,
        0x05,
        0x06,  // numufacturer id
        0x03,
        0x04,  // product type
        0x01,
        0x02  // product id
    };

    EXPECT_CALL( *this, fcSendData( 3, 2, expectedReport, false, false, Encapsulation_Sec_UNAUTHENTICATED ) );

    onMessageReceived(
        3, 2, { COMMAND_CLASS_MANUFACTURER_SPECIFIC, MANUFACTURER_SPECIFIC_GET }, Encapsulation_Sec_UNAUTHENTICATED );
}

TEST_F( SupportingFunctionalityTestSuite, ZWAVEPLUS_INFO_GET_test ) {
    ON_CALL( getOwner(), fillZWavePlusInfo( _ ) ).WillByDefault( Invoke( []( znet_zwaveplus_info_report_t& report ) {
        report.installer_icon = 0x0304;
        report.user_icon = 0x0506;
    } ) );

    const std::vector<uint8_t> expectedReport = {
        COMMAND_CLASS_ZWAVEPLUS_INFO,
        ZWAVEPLUS_INFO_REPORT,
        ZWAVEPLUS_INFO_VERSION_V2,                          // zwave plus info version
        ROLE_TYPE_CONTROLLER_CENTRAL_STATIC,                // role type
        ZWAVEPLUS_INFO_REPORT_NODE_TYPE_ZWAVEPLUS_NODE_V2,  // node type
        0x03,
        0x04,  // installer icon
        0x05,
        0x06  // user icon
    };

    EXPECT_CALL( *this, fcSendData( 3, 2, expectedReport, false, false, Encapsulation_Sec_2 ) );

    onMessageReceived( 3, 2, { COMMAND_CLASS_ZWAVEPLUS_INFO, ZWAVEPLUS_INFO_GET }, Encapsulation_Sec_2 );
}

}  // namespace ut
}  // namespace znet
