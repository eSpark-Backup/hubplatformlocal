#include "../moc/cc/Security2.h"
#include "znet_node_add_fixture.hpp"

TEST_F( NodeAddTestSuite, znet_inclusion ) {
    runZnetEventLoop();

    m_expectedNhid = { 0 };
    m_device0->inclusionSetReady();

    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    runZnetEventLoop();
    ASSERT_TRUE( m_nodeAddResultEnter );
}

TEST_F( NodeAddTestSuite, znet_inclusion_smart_start ) {
    runZnetEventLoop();

    ::znet::DskDerivedHomeId DSK = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
    m_hub->setDskDerivedHomeId( DSK );
    m_expectedNhid = { 0x11, 0x22, 0x33, 0x44 };
    m_device0->addCC( { std::make_shared<Security2>() } );
    m_device0->inclusionSetReady();

    ASSERT_EQ( znet_node_add_dsk_to_network( DSK.data() ), ZNET_ERR_OK );
    runZnetEventLoop();
    ASSERT_TRUE( m_nodeAddResultEnter );
}
