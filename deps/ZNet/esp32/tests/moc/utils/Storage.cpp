#include "Storage.h"

#include "assert.h"
#include "plog.h"
#include "MainFixture.h"

#define LOG_LOCAL_LEVEL P_LOG_VERBOSE
const char logtag[] = "Store";

Storage::Storage( Version version )
: m_data( m_store_size, 0 )
, m_version( version ) {
    m_data[0] = 0x5A;
    m_data[1] = 0x4E;
    m_data[2] = 0x45;
    m_data[3] = 0x54;
    m_data[4] = 0;
    m_data[5] = 0;
    m_data[6] = 1;
    m_data[7] = 0;
}

int Storage::save( size_t offset, const void* data, size_t size, void* arg ) {
    std::shared_ptr<Storage> pthis = static_cast<StorageInterface*>( arg )->getStorage();
    P_LOGD( logtag, "znet_store_save: offset: %zu, data: %p, size: %zu, arg: %p", offset, data, size, arg );

    assert( data != 0 );
    assert( size != 0 );
    assert( size < m_store_size );
    // assert( offset < m_store_size );

    uint8_t* p = (uint8_t*)data;
    for( size_t i = 0; i < size; i++ ) {
        pthis->m_data[i] = p[i];
    }

    return 0;
}

int Storage::load( size_t offset, void* data, size_t size, void* arg ) {
    std::shared_ptr<Storage> pthis = static_cast<StorageInterface*>( arg )->getStorage();
    P_LOGD( logtag, "znet_store_load: offset: %zu, data: %p, size: %zu, arg: %p", offset, data, size, arg );

    assert( data != 0 );
    assert( size != 0 );
    assert( size < m_store_size );
    //  assert( offset < m_store_size );

    uint8_t* p = (uint8_t*)data;
    for( size_t i = 0; i < size; i++ ) {
        p[i] = pthis->m_data[i];
    }

    return 0;
}

int Storage::reset( void* arg ) {
    std::shared_ptr<Storage> pthis = static_cast<StorageInterface*>( arg )->getStorage();
    P_LOGD( logtag, "znet_store_reset: arg: %p", arg );

    std::fill( pthis->m_data.begin(), pthis->m_data.end(), 0 );

    // TODO

    return 0;
}
