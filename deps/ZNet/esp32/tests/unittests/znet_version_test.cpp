#include "../moc/cc/Version.h"
#include "PrimaryControllerTestSuite.h"
#include "gtest/gtest.h"

static const char* logtag = "znet_version";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

TEST_F( PrimaryControllerTestSuite, znet_version ) {
    static uint8_t deViceId = 3;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    /* Add command classes */
    dev0->addCC( { std::make_shared<Version>() } );

    /* znet_init */
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( int err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
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

    /* version_get */
    P_LOGI( logtag, "Test version_get" );
    static bool versionResultEnter = false;
    static uint8_t ZWaveLibraryType = 1;
    static uint8_t ZWaveProtocolVersion = 1;
    static uint8_t ZWaveProtocolSubVersion = 0;
    static uint8_t ApplicationVersion = 1;
    static uint8_t ApplicationSubVersion = 0;

    m_callbacks.node_cmd_version_result =
        []( int err, znet_node_id_t node_id, const znet_version_report_t* value, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        versionResultEnter = true;
        P_LOGI( logtag,
                "node_cmd_version_result: \n\t"
                "err: %i\n\t"
                "node_id: 0x%02X\n\t"
                "lib_type:       0x%02X\n\t"
                "proto:          %d.%d\n\t"
                "hard_ver:       %d\n\t"
                "firms_count:    %d\n\t"
                "arg: %p\n",
                err,
                node_id,
                value->lib_type,
                value->proto.ver,
                value->proto.sub_ver,
                value->hard_ver,
                value->firms_count,
                arg );

        ASSERT_EQ( err, 0 ) << "Basic result fail!\n";
        ASSERT_EQ( node_id, deViceId );
        ASSERT_EQ( value->lib_type, ZWaveLibraryType );
        ASSERT_EQ( value->proto.ver, ZWaveProtocolVersion );
        ASSERT_EQ( value->proto.sub_ver, ZWaveProtocolSubVersion );
        ASSERT_EQ( value->firms_count, 1 );
        ASSERT_EQ( value->firms[0].ver, ApplicationVersion );
        ASSERT_EQ( value->firms[0].sub_ver, ApplicationSubVersion );
    };
    ASSERT_EQ( znet_node_cmd_version_get( deViceId ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( versionResultEnter );
}
