#pragma once

#include "MainFixture.h"

static const char* logtag = "znet_inclusion";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

class NodeAddTestSuite : public MainFixture {
protected:
    void SetUp() override {
        MainFixture::SetUp();
        m_deviceId = 3;
        m_nodeAddResultEnter = false;
        m_device0 = std::make_shared<Device>( 0xd1ac1b2d, m_deviceId );
        m_hub->deviceAdd( m_device0 );
        m_callbacks.arg = this;
        m_callbacks.node_add_result = znetNodeAddResult;
    }

    void runZnetEventLoop() {
        for( int i = 0; i < m_procIteration; ++i ) {
            P_LOGV( logtag, "call znet_proc(): %d", i );
            znet_proc();
        }
    }

    static void znetNodeAddResult( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) {
        if( err == ZNET_ERR_REQUEST_SENT_OK ) {
            return;
        }

        ASSERT_NE( arg, nullptr );

        auto self = reinterpret_cast<NodeAddTestSuite*>( arg );
        self->m_nodeAddResultEnter = true;

        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, self->m_deviceId );
        ASSERT_THAT( node_info->smart_start_data.nhid, ::testing::ElementsAreArray( self->m_expectedNhid ) );

        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    }

    std::shared_ptr<Device> m_device0;

    uint8_t m_deviceId;
    std::array<uint8_t, 4> m_expectedNhid;
    bool m_nodeAddResultEnter;
};
