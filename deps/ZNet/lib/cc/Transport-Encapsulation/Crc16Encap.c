
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

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// CRC16 Encap Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: command_class
// Command Class
//
// @param: command
// Command
//
// @param: msg_length
//
// @param: cmd_data
// Data
//
// @param: checksum
// Checksum (16 bits)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_crc16_encap( uint8_t node_id,
                                                    uint8_t msg_length,
                                                    const uint8_t* cmd_data,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg ) {
    uint16_t CRC16;
    uint8_t header[] = { COMMAND_CLASS_CRC_16_ENCAP, CRC_16_ENCAP };
    uint8_t header_len = sizeof header;

    uint8_t data_len = header_len + msg_length + 2;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    if( msg_length > 0 )
        memcpy( &data[header_len], cmd_data, msg_length );

    CRC16 = crc16( data, header_len + msg_length );
    int16_to_bytes( CRC16, &data[header_len + msg_length] );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, Encapsulation_None );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// CRC16 Encap Command Encrypt
void ICACHE_FLASH_ATTR znet_cc_crc16_encrypt( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data ) {
    print_hex_buf( cc_data, cc_data_len );
    uint16_t crc;
    uint8_t answer = false;
    encap_type_t secur = get_security_by_id( node_id ) & ALL_SECURITY;
    crc = crc16( cc_data, cc_data_len - 2 );
    uint16_t check_sum = cc_data[cc_data_len - 1] + cc_data[cc_data_len - 2] * 0x100;

    if( crc == check_sum ) {
        ZNet_ApplicationCommandHandler( (uint8_t*)&cc_data[2], cc_data_len - 4, node_id, Encapsulation_CRC16, 0 );
    }
    else {
        Log_Write( "ERROR. crc = %x, check_sum =%x\r\n", crc, check_sum );
    }
}
