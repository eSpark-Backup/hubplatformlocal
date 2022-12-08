#include "../moc/cc/Basic.h"
#include "../moc/cc/ManufacturerSpecific.h"
#include "PrimaryControllerTestSuite.h"
#include "gtest/gtest.h"

static const char* logtag = "znet_manufacturer_specific";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

TEST_F( PrimaryControllerTestSuite, znet_manufacturer_specific ) {
    static uint8_t deViceId = 3;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    /* Add command classes */
    static ManufacturerSpecific::idStruct_type manSpecId = {
        0x5588,  // manufacturerId
        0x7766,  // productTypeId
        0x4422,  // ProductId
    };

    dev0->addCC( { std::make_shared<ManufacturerSpecific>( manSpecId ) } );

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

    /* basic_get */
    P_LOGI( logtag, "Test znet_node_cmd_manufacturer_specific_get" );
    static bool ManufacturerSpecificEnter = false;
    m_callbacks.node_cmd_manufacturer_specific_result = []( int err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t,
                                                            const znet_manufacturer_specific_report_t* value,
                                                            void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        ManufacturerSpecificEnter = true;

        ASSERT_EQ( err, 0 ) << "Basic result fail!\n";
        ASSERT_EQ( node_id, deViceId );
        ASSERT_EQ( value->manufacturer_id, manSpecId.manufacturerId );
        ASSERT_EQ( value->product_type, manSpecId.productTypeId );
        ASSERT_EQ( value->product_id, manSpecId.productId );

        P_LOGI( logtag,
                "node_cmd_manufacturer_specific_result:\n\t"
                "err: %i\n\t"
                "node_id: 0x%02X\n\t"
                "value:\n\t\t"
                "manufacturer_id: 0x%02X\n\t\t"
                "product_type: 0x%02X\n\t\t"
                "product_id: 0x%02X\n\t"
                "arg: %p\n",
                err,
                node_id,
                value->manufacturer_id,
                value->product_type,
                value->product_id,
                arg );
    };
    ASSERT_EQ( znet_node_cmd_manufacturer_specific_get( deViceId, ZNET_CHANNEL_ID_ROOT ), ZNET_ERR_OK );

    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( ManufacturerSpecificEnter );
}
