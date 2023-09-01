#include "../moc/cc/Basic.h"
#include "MainFixture.h"
#include "gmock/gmock.h"

static const char* logtag = "znet_send_data_multi";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

namespace znet {
class ZSendDataMultiSuite : public MainFixture {
public:
    void SetUp() override {
        loginit();

        auto hub = std::make_shared<MockHub>( 0xd1ac1b2d, 0x44 );
        EXPECT_CALL( *hub, SendDataMultiWasReceived() ).Times( 1 );
        EXPECT_CALL( *hub, SendDataMultiBridgeWasReceived() ).Times( 0 );
        m_store = std::make_shared<Storage>( Storage::Version::v0_1 );

        m_hub = static_cast<std::shared_ptr<ZwaveHub>>( hub );
        m_hub->setLibraryType( 7 /*ZW_LIB_CONTROLLER_BRIDGE*/ );

        EXPECT_EQ( znet_init( &m_callbacks ), ZNET_ERR_OK );
        for( int i = 0; i < m_procIteration; ++i ) {
            znet_proc();
        }
    }
};

TEST_F( MainFixture, znet_send_data_multi ) {
    P_LOGI( logtag, "Send Data Multi" );

    static uint8_t deViceId[2] = { 3, 7 };
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId[0] );
    m_hub->deviceAdd( dev0 );

    /* Add command classes */
    dev0->addCC( { std::make_shared<Basic>() } );

    /* znet_init */
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add 1" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId[0] );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );
    dev0->inclusionClearReady();

    std::shared_ptr<Device> dev1 = std::make_shared<Device>( 0xd1ac1b2d, deViceId[1] );
    m_hub->deviceAdd( dev1 );

    /* Add command classes */
    dev1->addCC( { std::make_shared<Basic>() } );

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add 2" );
    dev1->inclusionSetReady();
    m_callbacks.node_add_result = []( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId[1] );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );
    dev1->inclusionClearReady();

    /* basic_set */
    P_LOGI( logtag, "Test basic_set multicast" );
    static uint8_t basicVal = 0x58;
    ASSERT_EQ( znet_node_cmd_basic_set_multi( 2, deViceId, ZNET_CHANNEL_ID_ROOT, basicVal, true ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* basic_get */
    P_LOGI( logtag, "Test basic_get for node 1" );
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
        ASSERT_EQ( node_id, deViceId[0] );
        ASSERT_EQ( currentValue, basicVal );
        ASSERT_EQ( targetValue, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN );
        ASSERT_EQ( duration, 0 );
        ASSERT_EQ( channel_id, ZNET_CHANNEL_ID_ROOT );
    };
    ASSERT_EQ( znet_node_cmd_basic_get( deViceId[0], ZNET_CHANNEL_ID_ROOT ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( basicResultEnter );

    /* basic_get */
    P_LOGI( logtag, "Test basic_get for node 2" );
    basicResultEnter = false;
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
        ASSERT_EQ( node_id, deViceId[1] );
        ASSERT_EQ( currentValue, basicVal );
        ASSERT_EQ( targetValue, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN );
        ASSERT_EQ( duration, 0 );
        ASSERT_EQ( channel_id, ZNET_CHANNEL_ID_ROOT );
    };
    ASSERT_EQ( znet_node_cmd_basic_get( deViceId[1], ZNET_CHANNEL_ID_ROOT ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( basicResultEnter );
}
}  // namespace znet
