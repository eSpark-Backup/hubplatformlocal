#include "MainFixture.h"
#include "znet/znet_utils.hpp"
#include <ctime>
#include <inttypes.h>

// TODO
static const char* logtag = "mainFixture";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

MainFixture::MainFixture()
: MainFixture( 0x77 ) {}

MainFixture::MainFixture( uint8_t myNodeId )
: m_myNodeId( myNodeId ) {
    memset( &m_callbacks, 0, sizeof( m_callbacks ) );
    m_callbacks.arg = this;
    m_callbacks.alloc = znetAlloc;
    m_callbacks.clock = znetClock;
    m_callbacks.log = []( int lvl, const char* format, va_list list, void* arg ) -> void { vprintf( format, list ); };
    m_callbacks.uart_write = znetUartWrite;
    m_callbacks.uart_read = znetUartRead;
    m_callbacks.store_save = m_store->save;
    m_callbacks.store_load = m_store->load;
    m_callbacks.store_reset = m_store->reset;
    m_callbacks.node_add_invoked = nodeAddInvoked;
    m_callbacks.check_node_supports_command_class = znetCheckNodeSupportsCommandClass;
}

void MainFixture::SetUp() {
    loginit();

    m_hub = std::make_shared<ZwaveHub>( 0xd1ac1b2d, m_myNodeId );
    m_store = std::make_shared<Storage>( Storage::Version::v0_1 );

    EXPECT_EQ( znet_init( &m_callbacks ), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        znet_proc();
    }
}

void MainFixture::TearDown() {
    znet_deinit();
    EXPECT_EQ( getUnfreeBlocksNum(), 0 );  // Check memory allocation
}

void MainFixture::loginit() {
    setvbuf( stdout, NULL, _IONBF, 0 );  // Disable stdout buffering
    plog_setVprintf( vsnprintf );
    plog_setWrite( write );
    static uint32_t count = 0;
    plog_setTimestamp( []() -> uint32_t { return count++; } );
}

std::shared_ptr<Storage> MainFixture::getStorage() {
    return m_store;
}

void MainFixture::printAllocInfo() {
    P_LOGD( logtag, "unfree %zu blocks", m_allocInfo.size() );
    for( auto x : m_allocInfo ) {
        P_LOGD( logtag, "\tblock %p: %zu bytes", x.first, x.second );
    }
}

size_t MainFixture::getUnfreeBlocksNum() {
    return m_allocInfo.size();
}

void* MainFixture::znetAlloc( void* ptr, size_t size, void* arg ) {
    MainFixture* pthis = static_cast<MainFixture*>( arg );

    if( size != 0 ) {
        void* p = realloc( ptr, size );
        assert( p != NULL );
        pthis->m_allocInfo[p] = size;
        if( ( (uint64_t)p & 0xFFF ) == 0x760 ) {
            p = p;  // Hook
        }
        return p;
    }
    else if( ptr ) {
        pthis->m_allocInfo.erase( ptr );
        free( ptr );
    }

    return nullptr;
}

uint64_t MainFixture::znetClock( void* arg ) {
    MainFixture* pthis = static_cast<MainFixture*>( arg );
    return pthis->m_znetClockCounter++;
}

int MainFixture::znetUartWrite( const void* data, size_t size, size_t* ret_size, void* arg ) {
    MainFixture* pthis = static_cast<MainFixture*>( arg );
    P_LOGV( logtag, "uart_write: %p, size: %lu, ret_size: %p, arg: %p", data, size, ret_size, arg );
    if( !data ) {
        if( !ret_size ) {
            return -1;
        }

        *ret_size = m_defaultZnetUartTxBuffSize;
        return 0;
    }

    pthis->m_hub->rxData( data, size );

    if( ret_size ) {
        *ret_size = size;
        P_LOGV( logtag, "uart_write: ret_size: %" PRIdMAX, *ret_size );
    }
    return 0;
}

int MainFixture::znetUartRead( void* data, size_t size, size_t* ret_size, void* arg ) {
    MainFixture* pthis = static_cast<MainFixture*>( arg );
    P_LOGV( logtag, "uart_read: %p, size: %lu, ret_size: %p, arg: %p", data, size, ret_size, arg );
    if( !data ) {
        if( !ret_size ) {
            return -1;
        }

        *ret_size = pthis->m_hub->getTxDataLength();
        P_LOGV( logtag, "uart_read: ret_size: %" PRIdMAX, *ret_size );
        return 0;
    }

    if( !size ) {
        assert( !"uart read: invalid args!" );
        return -1;
    }

    uint8_t readed = pthis->m_hub->txData( data, size );

    if( ret_size ) {
        *ret_size = readed;
        P_LOGV( logtag, "uart_read: ret_size: %" PRIdMAX, *ret_size );
    }

    return 0;
}

void MainFixture::znPrintNodeInfo( const znet_nodeinfo_t* node_info ) {
    printf( "Node info ->\n"
            "    id: %hhu\n"
            "    security: %hhu\n"
            "    basic: %hhu\n"
            "    generic: %hhu\n"
            "    specific: %hhu\n"
            "    is_smart_start: %s\n"
            "    commands count: %hhu\n"
            "\n",
            node_info->node_id,
            node_info->security,
            node_info->basic,
            node_info->generic,
            node_info->specific,
            ::znet::utils::boolToYesNo( node_info->smart_start_data.data != nullptr ),
            node_info->commands_count );
    znPrintHex( node_info->commands, node_info->commands_count * sizeof( znet_command_class_t ) );
}

void MainFixture::znPrintHex( const void* src, size_t data_size ) {
    const uint8_t* data = (uint8_t*)src;
    if( !data || !data_size )
        return;
    while( data_size ) {
        printf( "%p", data );
        for( size_t i = 0; data_size && ( i < 32 ); ++i, ++data, --data_size ) {
            printf( " %02hhX", *( (uint8_t*)data ) );
        }
        printf( "\n" );
    }
}

void MainFixture::nodeAddInvoked( void* arg ) {}

void MainFixture::onMessageReceived( uint8_t nodeId,
                                     uint8_t channelId,
                                     const std::vector<uint8_t>& data,
                                     uint8_t security,
                                     uint8_t messageType ) {
    ASSERT_TRUE( data.size() >= 2 );

    const auto report = ZCommandClass_FindCCReportByCCData( data.data() );

    EXPECT_EQ( ZCommandClass_FindTypeByCCData( data.data() ), messageType );

    ASSERT_TRUE( report );

    struct _ZFunction func = {};
    func._znet = &znet_instance;
    func._node_id = nodeId;
    func._endpoint = channelId;
    func._security = security;

    std::vector<uint8_t> frame( 7 + data.size() );
    func._znet->_frame._data = frame.data();

    func._znet->_frame._len = frame.size();
    func._znet->_frame._size = frame.size();

    ( *report )( &func, nodeId, static_cast<int>( data.size() ), data.data() );
}

bool MainFixture::znetCheckNodeSupportsCommandClass( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t endpoint,
                                                     znet_command_class_t commandClassId,
                                                     void* arg ) {
    return true;
}
