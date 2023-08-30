
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

#include "logger.h"
#include "znet_fc_private.h"
#include "S2_wrap.h"
//#include "transport_service2_external.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// First Segment Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: datagram_size_1
// datagram_size_1 (3 bits)
//
// @param: datagram_size_2
// datagram_size_2
//
// @param: ext_avaliable
//
// @param: ext
// Ext (1 bits)
//
// @param: session_id
// Session ID
//
// @param: seq_n
// Sequence No (4 bits)
//
// @param: header_ext_avaliable
//
// @param: header_ext_length
// Header Extension Length
//
// @param: header_extension
// Header Extension
//
// @param: size_payload
//
// @param: payload
// Payload
//
// @param: check
// Frame Check Sequence
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_first_segment( uint8_t node_id,
                                                      uint8_t datagram_size_1,
                                                      uint8_t datagram_size_2,
                                                      bool ext_avaliable,
                                                      bool ext,
                                                      uint8_t session_id,
                                                      uint8_t seq_n,
                                                      bool header_ext_avaliable,
                                                      uint8_t header_ext_length,
                                                      const uint8_t* header_extension,
                                                      uint8_t size_payload,
                                                      const uint8_t* payload,
                                                      uint16_t check,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg ) {
    uint8_t header[] = {
        COMMAND_CLASS_TRANSPORT_SERVICE_V2, COMMAND_FIRST_SEGMENT_V2 | ( datagram_size_1 & 0x07 ), datagram_size_2, 0
    };
    header[3] = ext_avaliable ? ( ( session_id & 0x0F ) << 4 ) | ( ext ? 0x08 : 0x00 ) : ( seq_n & 0x0F );
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + ( header_ext_avaliable ? ( header_ext_length + 1 ) : 0 ) + size_payload + 2;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_OK;

    memcpy( &data[0], header, header_len );
    uint8_t offset = 0;
    if( header_ext_avaliable && ( header_ext_length > 0 ) ) {
        data[header_len + 1] = header_ext_length;
        memcpy( &data[header_len], header_extension, header_ext_length );
        offset = header_ext_length + 1;
    }
    if( size_payload )
        memcpy( &data[header_len + offset], payload, size_payload );
    int16_to_bytes( check, &data[header_len + offset + size_payload] );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, Encapsulation_None );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Segment Complete Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: session_id
// Session ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_segment_complete( uint8_t node_id,
                                                         uint8_t session_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_TRANSPORT_SERVICE_V2, COMMAND_SEGMENT_COMPLETE_V2, ( session_id & 0x0F ) << 4 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

//----------------------------------------------------------------------------
// Segment Request Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: session_id
// Session ID
//
// @param: datagram_offset_1
// datagram_offset_1
//
// @param: datagram_offset_2
// datagram_offset_2
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_segment_request( uint8_t node_id,
                                                        uint8_t session_id,
                                                        uint8_t datagram_offset_1,
                                                        uint8_t datagram_offset_2,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_TRANSPORT_SERVICE_V2,
                       COMMAND_SEGMENT_REQUEST_V2,
                       ( ( session_id & 0x0F ) << 4 ) | ( datagram_offset_1 & 0x07 ),
                       datagram_offset_2 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

//----------------------------------------------------------------------------
// Segment Wait Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: pending_fragments
// pending_fragments
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_segment_wait( uint8_t node_id,
                                                     uint8_t pending_fragments,
                                                     ZFunction_Callback Callback,
                                                     void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_TRANSPORT_SERVICE_V2, COMMAND_SEGMENT_WAIT_V2, pending_fragments };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

//----------------------------------------------------------------------------
// Subsequent Segment Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: datagram_size_1
// datagram_size_1 (3 bits)
//
// @param: datagram_size_2
// datagram_size_2
//
// @param: ext
// Ext (1 bits)
//
// @param: session_id
// Session ID
//
// @param: seq_n
// Sequence No (4 bits)
//
// @param: session_id
// Session ID
//
// @param: header_ext_length
// Header Extension Length
//
// @param: header_extension
// Header Extension
//
// @param: size_payload
//
// @param: payload
// Payload
//
// @param: check
// Frame Check Sequence
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_subsequent_segment( uint8_t node_id,
                                                           uint8_t datagram_size_1,
                                                           uint8_t datagram_size_2,
                                                           bool ext_avaliable,
                                                           bool ext,
                                                           uint8_t session_id,
                                                           uint8_t seq_n,
                                                           uint8_t datagram_offset_1,
                                                           uint8_t datagram_offser_2,
                                                           bool header_ext_avaliable,
                                                           uint8_t header_ext_length,
                                                           const uint8_t* header_extension,
                                                           uint8_t size_payload,
                                                           const uint8_t* payload,
                                                           uint16_t check,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg ) {
    uint8_t header[] = { COMMAND_CLASS_TRANSPORT_SERVICE_V2,
                         COMMAND_SUBSEQUENT_SEGMENT_V2 | ( datagram_size_1 & 0x07 ),
                         datagram_size_2,
                         0,
                         datagram_offser_2 };
    header[3] = ext_avaliable ? ( ( session_id & 0x0F ) << 4 ) | ( ext ? 0x08 : 0x00 ) | ( datagram_offset_1 & 0x07 ) :
                                ( ( ( seq_n & 0x0F ) << 3 ) | ( datagram_offset_1 & 0x07 ) );
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + ( header_ext_avaliable ? ( header_ext_length + 1 ) : 0 ) + size_payload + 2;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    uint8_t offset = 0;
    if( header_ext_avaliable && ( header_ext_length > 0 ) ) {
        data[header_len + 1] = header_ext_length;
        memcpy( &data[header_len], header_extension, header_ext_length );
        offset = header_ext_length + 1;
    }
    if( size_payload > 0 )
        memcpy( &data[header_len + offset], payload, size_payload );
    int16_to_bytes( check, &data[header_len + offset + size_payload] );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, Encapsulation_None );
    Heap_Free( data );

    return result;
}

extern void TransportService_ApplicationCommandHandler( ts_param_t* p, uint8_t* pCmd, uint8_t cmdLength );

//----------------------------------------------------------------------------
// Transport Recive
//
void ICACHE_FLASH_ATTR znet_cc_transport_receive( const ZFunction UNUSED( func ),
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    Log_Write( "znet_cc_transport_receive  \n" );
    ts_param_t* p = (ts_param_t*)Heap_Alloc( sizeof( ts_param_t ) );
    if( p == NULL )
        return;

    p->snode = node_id;
    p->dnode = MY_NODE_ID;
    p->tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
    //  p->rx_flags = rx_value->bytes[0];
    p->rx_flags = 0;
    p->scheme = NO_SCHEME;

    TransportService_ApplicationCommandHandler( p, (uint8_t*)cc_data, cc_data_len );
    Heap_Free( p );
}
