#include "../moc/cc/Basic.h"
#include "MainFixture.h"
#include "gtest/gtest.h"

static const char* logtag = "znet_basic";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

TEST_F( MainFixture, znet_basic ) {
    static uint8_t deViceId = 3;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    /* Add command classes */
    dev0->addCC( { std::make_shared<Basic>() } );

    /* znet_init */
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

    /* basic_set */
    P_LOGI( logtag, "Test basic_set" );
    static uint8_t basicVal = 0x28;
    ASSERT_EQ( znet_node_cmd_basic_set( deViceId, ZNET_CHANNEL_ID_ROOT, basicVal ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* basic_get */
    P_LOGI( logtag, "Test basic_get" );
    static bool basicResultEnter = false;
    m_callbacks.node_cmd_basic_result = []( znet_error_t err,
                                            znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            znet_cmd_basic_value_t currentValue,
                                            znet_cmd_basic_value_t targetValue,
                                            uint8_t duration,
                                            void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        basicResultEnter = true;
        P_LOGI( logtag,
                "node_cmd_basic_result: "
                "err: %i, "
                "node_id: 0x%02X, "
                "channel_id: 0x%02X, "
                "currentValue: 0x%02X, "
                "targetValue: 0x%02X, "
                "duration: 0x%02X, "
                "arg: %p\n",
                err,
                node_id,
                channel_id,
                currentValue,
                targetValue,
                duration,
                arg );

        ASSERT_EQ( err, 0 ) << "Basic result fail!\n";
        ASSERT_EQ( node_id, deViceId );
        ASSERT_EQ( currentValue, basicVal );
        ASSERT_EQ( targetValue, ZNET_CMD_BASIC_VALUE_UNKNOWN );
        ASSERT_EQ( duration, 0 );
        ASSERT_EQ( channel_id, ZNET_CHANNEL_ID_ROOT );
    };
    ASSERT_EQ( znet_node_cmd_basic_get( deViceId, ZNET_CHANNEL_ID_ROOT ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( basicResultEnter );
}
