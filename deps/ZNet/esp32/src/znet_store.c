/**
 * @file znet_store.c
 * @date 16 Mar 2018
 * @brief TBD.
 *
 * Storage format:
 * 0     : HEADER -> 64 bytes
 * 64    : massive of node info -> 231 count * 64 bytes
 * 14848 : S2 keystore -> 128 bytes
 * 14976 : massive of security supported commands -> 231 count * 64 bytes
 */

/// INFO: pair
#include "znet_store.h"

/// INFO: crt & system
#include <stdint.h>
#include <string.h>
#include <assert.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"

/// INFO: internal
#include "heap.h"

/// TODO: move to utils...
#define ZNET_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

#define ZNET_STORE_MAGIC 0x54454E5A
#define ZNET_STORE_VER_MAJ 0
#define ZNET_STORE_VER_MIN 1

extern const uint8_t max_association_groups;
extern const uint8_t max_groups_supported;
extern const uint8_t max_nodes_supported;

/**
 * @brief TBD.
 *
 * Max header size is 64 bytes
 */
typedef struct znet_store_head_t {
    uint32_t magic;   /**< MAGIC */
    uint16_t ver_maj; /**< Major version of store */
    uint16_t ver_min; /**< Minor version of store */
} znet_store_head_t;

/// TODO: implement compile time assert (
/// http://www.pixelbeat.org/programming/gcc/static_assert.html ) #if (
/// ZNET_STORE_HEAD_SIZE_MAX < sizeof( znet_store_head_t ) ) #error
/// "sizeof(znet_store_head_t) > ZNET_STORE_HEAD_SIZE_MAX, Is there something
/// wrong !!!" #endif

znet_error_t znet_store_reset( void ) {
    znet_store_head_t head = {
        .magic = ZNET_STORE_MAGIC,
        .ver_maj = ZNET_STORE_VER_MAJ,
        .ver_min = ZNET_STORE_VER_MIN,
    };

    if( znet_cb->store_reset( znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    if( znet_cb->store_save( 0, &head, sizeof( head ), znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    return ZNET_ERR_OK;
}

static znet_error_t _znet_store_convert( void ) {
    /// TODO: try convert store to current version, now reset only
    if( znet_store_reset() )  // try reset store
        return ZNET_ERR_FAULT;

    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

static inline size_t _znet_store_header_end( void ) {
    return ZNET_STORE_HEAD_SIZE_MAX;
}

znet_error_t znet_store_init( void ) {
    znet_store_head_t head = {};

    const size_t offset = 0;

    if( znet_cb->store_load( offset, &head, sizeof( head ), znet_cb->arg ) ) {
        if( znet_store_reset() )  // try reset store
            return ZNET_ERR_FAULT;

        if( znet_cb->store_load( offset, &head, sizeof( head ), znet_cb->arg ) )
            return ZNET_ERR_FAULT;
    }
    //  TODO    till znet store is not FILE
    if( head.magic != ZNET_STORE_MAGIC ) {
        if( znet_store_reset() )  // try reset store
            return ZNET_ERR_FAULT;

        if( znet_cb->store_load( offset, &head, sizeof( head ), znet_cb->arg ) )
            return ZNET_ERR_FAULT;
    }

    if( head.ver_maj != ZNET_STORE_VER_MAJ || head.ver_min != ZNET_STORE_VER_MIN ) {
        if( _znet_store_convert() )
            return ZNET_ERR_FAULT;

        if( znet_cb->store_load( offset, &head, sizeof( head ), znet_cb->arg ) )
            return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

/// TODO: implement compile time assert ( see TODO above )
/// #if ( ( ( ZNET_NODE_ID_MAX - ZNET_NODE_ID_MIN ) != 231 ) || (
/// ZNET_NODE_ID_MIN != 1 ) ) #error "Please check ZNET_NODE_ID_*, Is there
/// something wrong !!!" #endif

static inline size_t _znet_store_node_info_offset( znet_node_id_t node_id ) {
    return _znet_store_header_end() + ( node_id - ZNET_NODE_ID_MIN ) * ZNET_STORE_NODE_INFO_SIZE_MAX;
}

static inline size_t _znet_store_node_info_end( void ) {
    return _znet_store_node_info_offset( ZNET_NODE_ID_MAX + 1 );
}

znet_error_t znet_store_node_info_set( const znet_nodeinfo_t* node_info ) {
    assert( node_info );
    const size_t node_info_size = znet_node_info_size( node_info );

    if( node_info_size > ZNET_STORE_NODE_INFO_SIZE_MAX || node_info->node_id < ZNET_NODE_ID_MIN ||
        node_info->node_id > ZNET_NODE_ID_MAX ) {
        ZNET_LOGD( "[%s] Invalid node ID!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    const size_t offset = _znet_store_node_info_offset( node_info->node_id );
    uint8_t buff[ZNET_STORE_NODE_INFO_SIZE_MAX];
    if( znet_store_node_info_get( node_info->node_id, (znet_nodeinfo_t*)&buff, ZNET_STORE_NODE_INFO_SIZE_MAX ) )
        return ZNET_ERR_COMM;
    const size_t buff_node_info_size = znet_node_info_size( (znet_nodeinfo_t*)&buff );
    if( node_info_size == buff_node_info_size &&
        memcmp( buff, node_info, ZNET_MIN( node_info_size, buff_node_info_size ) ) == 0 )
        return 0;  // not need update in store
    memset( buff, 0, ZNET_STORE_NODE_INFO_SIZE_MAX );
    memcpy( buff, node_info, ZNET_MIN( node_info_size, ZNET_STORE_NODE_INFO_SIZE_MAX ) );

    if( znet_cb->store_save( offset, buff, ZNET_STORE_NODE_INFO_SIZE_MAX, znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    return ZNET_ERR_OK;
}

znet_error_t znet_store_node_info_get( znet_node_id_t node_id, znet_nodeinfo_t* node_info, size_t size ) {
    assert( node_info );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        ZNET_LOGD( "[%s] Invalid node ID!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    const size_t offset = _znet_store_node_info_offset( node_id );
    if( znet_cb->store_load( offset, node_info, ZNET_MIN( size, ZNET_STORE_NODE_INFO_SIZE_MAX ), znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    if( znet_node_info_size( node_info ) > size && znet_node_info_size( node_info ) <= ZNET_STORE_NODE_INFO_SIZE_MAX ) {
        memset( node_info, 0, size );  // buffer small: clear
        return ZNET_ERR_INVAL;
    }

    /// TODO: implement function for validate znet_nodeinfo_t
    if( znet_node_info_size( node_info ) > ZNET_STORE_NODE_INFO_SIZE_MAX || node_info->node_id < ZNET_NODE_ID_MIN ||
        node_info->node_id > ZNET_NODE_ID_MAX || node_info->generic == 0 || node_info->node_id != node_id )
        memset( node_info, 0, size );  // node invalid: clear

    return ZNET_ERR_OK;
}

znet_error_t znet_store_node_info_clean( znet_node_id_t node_id ) {
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        ZNET_LOGD( "[%s] Invalid node ID!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    size_t offset = _znet_store_node_info_offset( node_id );
    uint8_t zero[ZNET_STORE_NODE_INFO_SIZE_MAX] = {};

    if( znet_cb->store_save( offset, zero, ZNET_STORE_NODE_INFO_SIZE_MAX, znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

static inline size_t _znet_store_S2_keystore_end( void ) {
    return _znet_store_node_info_end() + ZNET_STORE_S2_KEYSTORE_SIZE_MAX;
}

znet_error_t znet_store_S2_keystore_set( void* data, size_t size ) {
    assert( data );
    assert( size );

    const size_t offset = _znet_store_node_info_end();
    uint8_t buff[ZNET_STORE_S2_KEYSTORE_SIZE_MAX] = {};
    memcpy( buff, data, ZNET_MIN( size, ZNET_STORE_S2_KEYSTORE_SIZE_MAX ) );
    if( znet_cb->store_save( offset, buff, ZNET_STORE_S2_KEYSTORE_SIZE_MAX, znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}

znet_error_t znet_store_S2_keystore_get( void* data, size_t size ) {
    assert( data );
    assert( size );

    const size_t offset = _znet_store_node_info_end();
    if( znet_cb->store_load( offset, data, ZNET_MIN( size, ZNET_STORE_S2_KEYSTORE_SIZE_MAX ), znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

static inline size_t _znet_store_security_supported_offset( znet_node_id_t node_id ) {
    return _znet_store_S2_keystore_end() + ( node_id - ZNET_NODE_ID_MIN ) * ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX;
}

static inline size_t _znet_store_security_supported_end( void ) {
    return _znet_store_security_supported_offset( ZNET_NODE_ID_MAX + 1 );
}

znet_error_t znet_store_security_supported_set( znet_node_id_t node_id,
                                                const znet_security_supported_report_t* security_supported,
                                                size_t follow_parts ) {
    assert( security_supported );
    if( znet_security_supported_size( security_supported ) >
        ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX - sizeof( uint8_t ) ) {
        ZNET_LOGD( "[%s] Invalid size!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        ZNET_LOGD( "[%s] Invalid node ID!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    size_t offset = _znet_store_security_supported_offset( node_id );
    uint8_t buff[ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX] = {};
    if( znet_cb->store_load( offset, buff, ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX, znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    uint8_t* _follow_parts = (uint8_t*)&buff;
    znet_security_supported_report_t* _security_supported = (znet_security_supported_report_t*)( _follow_parts + 1 );
    if( *_follow_parts ) {
        // merge
        const size_t ccount = _security_supported->commands_count;
        for( size_t i = 0; security_supported->commands_count; ++i ) {
            _security_supported->commands[ccount + i] = security_supported->commands[i];
        }
        _security_supported->commands_count = ccount + security_supported->commands_count;
    }
    else {
        // new
        *_follow_parts = follow_parts;
        memcpy( _security_supported,
                security_supported,
                ZNET_MIN( znet_security_supported_size( security_supported ),
                          ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX - sizeof( uint8_t ) ) );
    }
    if( znet_cb->store_save( offset, buff, ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX, znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }
    return ZNET_ERR_OK;
}

znet_error_t znet_store_security_supported_get( znet_node_id_t node_id,
                                                znet_security_supported_report_t* security_supported,
                                                size_t size ) {
    assert( security_supported );
    if( size < sizeof( znet_security_supported_report_t ) ) {
        ZNET_LOGD( "[%s] Invalid size!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        ZNET_LOGD( "[%s] Invalid node ID!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    size_t offset = _znet_store_security_supported_offset( node_id );
    uint8_t follow_parts = 0;
    if( znet_cb->store_load( offset,
                             &follow_parts,
                             ZNET_MIN( sizeof( follow_parts ), ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX ),
                             znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    if( follow_parts ) {
        memset( security_supported, 0, size );  // data in fill progress: clear
        return ZNET_ERR_INVAL;
    }

    if( znet_cb->store_load( offset + sizeof( uint8_t ),
                             security_supported,
                             ZNET_MIN( size, ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX - sizeof( uint8_t ) ),
                             znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    if( znet_security_supported_size( security_supported ) > size &&
        znet_security_supported_size( security_supported ) <=
            ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX - sizeof( uint8_t ) ) {
        memset( security_supported, 0, size );  // buffer small: clear
        return ZNET_ERR_INVAL;
    }

    /// TODO: implement function for validate znet_security_supported_report_t
    if( znet_security_supported_size( security_supported ) >
        ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX - sizeof( uint8_t ) )
        memset( security_supported, 0, size );  // supported invalid: clear

    return ZNET_ERR_OK;
}

znet_error_t znet_store_security_supported_clean( znet_node_id_t node_id ) {
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        ZNET_LOGD( "[%s] Invalid node ID!\n", __func__ );
        return ZNET_ERR_INVAL;
    }

    size_t offset = _znet_store_security_supported_offset( node_id );
    uint8_t zero[ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX] = {};
    if( znet_cb->store_save( offset, zero, ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX, znet_cb->arg ) )
        return ZNET_ERR_FAULT;

    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

static inline size_t _znet_store_associations_offset( uint8_t group_id ) {
    return _znet_store_security_supported_end() + group_id * ZNET_NODE_ID_MAX;
}

static inline size_t _znet_store_associations_end( void ) {
    return _znet_store_associations_offset( max_association_groups );
}

znet_error_t znet_store_associations_set( uint8_t group_id, const uint8_t* associations ) {
    const size_t offset = _znet_store_associations_offset( group_id );

    if( znet_cb->store_save( offset, associations, ZNET_NODE_ID_MAX, znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}

znet_error_t znet_store_associations_get( uint8_t group_id, uint8_t* associations ) {
    const size_t offset = _znet_store_associations_offset( group_id );

    if( znet_cb->store_load( offset, associations, ZNET_NODE_ID_MAX, znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

static inline size_t _znet_store_multichannel_associations_offset( uint8_t group_id ) {
    return _znet_store_associations_end() +
           group_id * max_nodes_supported * sizeof( VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG );
}

static inline size_t _znet_store_multichannel_associations_end( void ) {
    return _znet_store_multichannel_associations_offset( max_groups_supported );
}

znet_error_t znet_store_multichannel_associations_set( uint8_t group_id,
                                                       const VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG* associations ) {
    const size_t offset = _znet_store_multichannel_associations_offset( group_id );

    if( znet_cb->store_save( offset,
                             associations,
                             max_nodes_supported * sizeof( VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG ),
                             znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}

znet_error_t znet_store_multichannel_associations_get( uint8_t group_id,
                                                       VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG* associations ) {
    const size_t offset = _znet_store_multichannel_associations_offset( group_id );

    if( znet_cb->store_load( offset,
                             associations,
                             max_nodes_supported * sizeof( VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG ),
                             znet_cb->arg ) ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}
