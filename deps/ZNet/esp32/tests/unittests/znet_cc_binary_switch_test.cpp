#include "../moc/cc/BinarySwitch.h"

#include <znet/znet_node.hpp>
#include <znet/cc_0x25_switch.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "MainFixture.h"
#include "znet_fc_send_data_mock.h"

namespace znet {
namespace ut {

static const char* logtag = "znet_binarySwitch";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

TEST_F( MainFixture, znet_binarySwitch ) {
    m_hub->setLibraryType( 7 /*ZW_LIB_CONTROLLER_BRIDGE*/ );

    static uint8_t deViceId = 3;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    /* Add command classes */
    dev0->addCC( { std::make_shared<BinarySwitch>() } );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );

    P_LOGI( logtag, "Test znet_node_cmd_binary_switch_set" );
    static uint8_t swichVal = 0xFF;
    ASSERT_EQ( znet_node_cmd_binary_switch_set( deViceId, ZNET_CHANNEL_ID_ROOT, swichVal ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    P_LOGI( logtag, "Test znet_node_cmd_binary_switch_get" );
    static bool binarySwitchEnter = false;
    m_callbacks.node_cmd_binary_switch_result = []( znet_error_t err,
                                                    znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_binary_switch_value_t value,
                                                    znet_cmd_binary_switch_value_t target_value,
                                                    uint8_t duration,
                                                    void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        binarySwitchEnter = true;
        P_LOGI( logtag,
                "node_cmd_binary_switch_result: "
                "err: %i, "
                "node_id: 0x%02X, "
                "channel_id: 0x%02X, "
                "value: 0x%02X, "
                "arg: %p\n",
                err,
                node_id,
                channel_id,
                value,
                arg );

        ASSERT_EQ( err, 0 ) << "binarySwitch result fail!\n";
        ASSERT_EQ( node_id, deViceId );
        ASSERT_EQ( value, swichVal );
        ASSERT_EQ( channel_id, ZNET_CHANNEL_ID_ROOT );
    };
    ASSERT_EQ( znet_node_cmd_binary_switch_get( deViceId, ZNET_CHANNEL_ID_ROOT ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( binarySwitchEnter );
}

class ZCcBinarySwitchSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
};

TEST_F( ZCcBinarySwitchSuite, znet_node_cmd_binary_switch_set_test ) {
    ensureNodeAndCc<ZCc0x25Switch>( 3, 0, 2 );

    EXPECT_CALL( *this,
                 fcSendData( 3,
                             0,
                             std::vector<uint8_t>{ ZNET_COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_SET, 0, 0xFF },
                             false,
                             false,
                             Encapsulation_None ) );
    EXPECT_EQ( znet_node_cmd_binary_switch_set( 3, 0, 0 ), ZNET_ERR_OK );

    EXPECT_CALL( *this,
                 fcSendData( 3,
                             0,
                             std::vector<uint8_t>{ ZNET_COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_SET, 0XFF, 0xFF },
                             false,
                             false,
                             Encapsulation_None ) );
    EXPECT_EQ( znet_node_cmd_binary_switch_set( 3, 0, 1 ), ZNET_ERR_OK );
}

}  // namespace ut
}  // namespace znet
