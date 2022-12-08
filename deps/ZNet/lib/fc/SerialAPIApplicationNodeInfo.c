#include <string.h>

#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"
#include "znet_lib.h"

#include "znet_lib_fc.h"
#include "znet_lib_cc_codes.h"

#include "logger.h"
#include "ZPlatform.h"

/*============================================================================
**    Defines for ApplicationNodeInformation field deviceOptionMask
**--------------------------------------------------------------------------*/
#define APPLICATION_NODEINFO_NOT_LISTENING 0x00
#define APPLICATION_NODEINFO_LISTENING 0x01
#define APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY 0x02
#define APPLICATION_FREQ_LISTENING_MODE_1000ms 0x10
#define APPLICATION_FREQ_LISTENING_MODE_250ms 0x20

/* clang-format off */
const uint8_t NIF_COMMON[] = {
    COMMAND_CLASS_ZWAVEPLUS_INFO,
    COMMAND_CLASS_CRC_16_ENCAP,
    COMMAND_CLASS_SECURITY,
    COMMAND_CLASS_SECURITY_2,
    COMMAND_CLASS_SUPERVISION,
    COMMAND_CLASS_TRANSPORT_SERVICE,
    COMMAND_CLASS_INCLUSION_CONTROLLER,
#ifndef CONFIG_UART_API
    COMMAND_CLASS_MULTI_CMD,
    COMMAND_CLASS_TIME,
#endif
};
// this CC present in NIF if device connected without security
const uint8_t NIF_NON_SECURITY[] = {
    COMMAND_CLASS_DEVICE_RESET_LOCALLY,
    COMMAND_CLASS_ASSOCIATION,
    COMMAND_CLASS_ASSOCIATION_GRP_INFO,
    COMMAND_CLASS_MANUFACTURER_SPECIFIC,
    COMMAND_CLASS_VERSION,
    COMMAND_CLASS_POWERLEVEL,
    COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3,
#ifndef CONFIG_UART_API
    COMMAND_CLASS_INDICATOR,
    COMMAND_CLASS_NODE_PROVISIONING,
    COMMAND_CLASS_FIRMWARE_UPDATE_MD,
#endif
};
/* clang-format on */
const uint8_t size_nif_common = sizeof( NIF_COMMON );
const uint8_t size_nif_non_security = sizeof( NIF_NON_SECURITY );

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SerialAPIApplicationNodeInfoInit( ZFunction this, void* initArg ) {
    uint8_t security = initArg ? ( *(uint8_t*)initArg ) : 0;

    /* clang-format off */
    uint8_t size = size_nif_common + (security ? 0 : size_nif_non_security) +
            6 /*message title + message end*/ +
            4 /*info + size*/;
    /* clang-format on */

    Log_Write( "ApplicationNodeInfoInit secutiry = %d, size = %d\r\n", security, size );

    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( size ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = size;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = size - 2;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = APPLICATION_NODEINFO_LISTENING | APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY;
    buffer->_data[5] = GENERIC_TYPE_STATIC_CONTROLLER;
    buffer->_data[6] = SPECIFIC_TYPE_GATEWAY;
    buffer->_data[7] = size - 10;
    uint8_t i = 0;
    uint8_t offset = 8;

    for( ; i < size_nif_common; ++i ) {
        if( NIF_COMMON[i] != COMMAND_CLASS_MARK )
            buffer->_data[offset + i] = NIF_COMMON[i];
        else
            break;
    }
    if( !security ) {
        memcpy( &buffer->_data[offset + i], NIF_NON_SECURITY, size_nif_non_security );
        offset += size_nif_non_security;
    }
    for( ; i < size_nif_common; ++i ) {
        buffer->_data[offset + i] = NIF_COMMON[i];
    }

    // data[ 21 ] = 0xFF; // callback_id
    // data[ 22 ] = 0xFF; // crc    return ZNET_ERR_OK;
    return ZNET_ERR_OK;
}

FuncResults_e __SerialAPIApplicationNodeInfoAck( ZFunction this ) {
    if( this->_Callback ) {
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSerialAPIApplicationNodeInfo = {
    0x03,  // _id
    "SerialAPIApplicationNodeInformation",
    __SerialAPIApplicationNodeInfoInit,  // _Init
    __SerialAPIApplicationNodeInfoAck,   // _ProcessACK
    NULL,                                // _ProcessRESPONSE
    NULL,                                // _ProcessCALLBACK
    NULL                                 // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_application_node_info( bool security,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSerialAPIApplicationNodeInfo, &security, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
