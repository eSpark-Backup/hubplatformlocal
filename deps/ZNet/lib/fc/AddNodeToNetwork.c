
#include "byte_ops.h"

//#include "ZControllerData.h"

//#include "znet_cc_private.h"

#include "znet.h"
#include "ZPlatform.h"

#include "znet_lib_fc.h"
#include "znet_lib.h"

#include "znet_private.h"

#include "logger.h"

#define ADD_NODE_STATUS_LEARN_READY 0x01        // Z-Wave protocol is ready to include new node.
#define ADD_NODE_STATUS_NODE_FOUND 0x02         // Z-Wave protocol detected node.
#define ADD_NODE_STATUS_ADDING_SLAVE 0x03       // Z-Wave protocol included a slave type node
#define ADD_NODE_STATUS_ADDING_CONTROLLER 0x04  // Z-Wave protocol included a controller type node
#define ADD_NODE_STATUS_PROTOCOL_DONE 0x05      // Z-Wave protocol completed operations related to inclusion (add)
#define ADD_NODE_STATUS_DONE 0x06               // All operations completed. Protocol is ready to return to idle state.
#define ADD_NODE_STATUS_FAILED 0x07             // Z-Wave protocol reports that inclusion (add) was not successful
#define ADD_NODE_STATUS_NOT_PRIMARY 0x23  // requested operation requires that the node is in primary controller state

extern void ICACHE_FLASH_ATTR AddNodeInvokedCallback();

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __AddNodeToNetworkCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 4 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }
    Log_Write( "Add node: data[3] = %d    size %d \r\n", data[3], size );

    switch( data[3] ) {
    case ADD_NODE_STATUS_LEARN_READY:
        Log_Write( "Add node: Push a button on the device to be added\r\n" );
        AddNodeInvokedCallback();
        return FuncResults_Repeat;

    case ADD_NODE_STATUS_NODE_FOUND:
        Log_Write( "Add node: Node found:%d\r\n", data[4] );
        return FuncResults_Repeat;

    case ADD_NODE_STATUS_ADDING_SLAVE:
        Log_Write( "Add node: Adding slave:%d\r\n", data[4] );
        if( size < 11 ) {
            Log_Write( "Add node: Callback" );
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        this->_dataSize = size - 2;
        if( ( this->_dataPtr = (uint8_t*)Heap_Alloc( this->_dataSize ) ) != NULL ) {
            memcpy( this->_dataPtr, &data[2], this->_dataSize );
        }
        return FuncResults_Repeat;

    case ADD_NODE_STATUS_ADDING_CONTROLLER:
        Log_Write( "Add node: Adding controller:%d\r\n", data[4] );
        if( size < 6 ) {  // return ( ( this->_req_attempts-- > 0 ) ? FuncResults_Restart :
                          // FuncResults_Done );
            Log_Write( "Add node: Callback" );
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        this->_dataSize = size - 2;
        if( ( this->_dataPtr = (uint8_t*)Heap_Alloc( this->_dataSize ) ) != NULL ) {
            memcpy( this->_dataPtr, &data[2], this->_dataSize );
        }
        return FuncResults_Repeat;

    case ADD_NODE_STATUS_PROTOCOL_DONE: {
        Log_Write( "Add node: Protocol done\r\n" );
        if( size < 6 ) {
            // return ( ( this->_req_attempts-- > 0 ) ? FuncResults_Restart :
            // FuncResults_Done );
            Log_Write( "Add node: Callback\r\n" );
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }

        Log_Write( "Add node: OnSuccess\r\n" );
        this->_ProcessCALLBACK = NULL;

        if( this->_Callback ) {
            if( this->_dataPtr ) {
                print_hex_buf( this->_dataPtr, this->_dataSize );
                ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, this->_dataPtr, this->_dataSize );
                Heap_Free( this->_dataPtr );
                this->_dataPtr = NULL;
            }
            else if( this->_dataSize )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_NOMEM, NULL, 0 );
            else
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_INVALID, NULL, 0 );
        }
        this->_dataSize = 0;
    }
        return FuncResults_Done;

    case ADD_NODE_STATUS_DONE:
        Log_Write( "Add node: Done \r\n" );

        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
        }
        return FuncResults_Done;

    case ADD_NODE_STATUS_FAILED: {
        Log_Write( "Add node: Failed\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
        return FuncResults_Done;
    }

    case ADD_NODE_STATUS_NOT_PRIMARY: {
        Log_Write( "Add node: Not Primary\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
        return FuncResults_Done;
    }

    default: {
        Log_Write( "Add node: Invalid status:%02x\r\n", data[3] );
        return FuncResults_Restart;
    }
    }
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __AddNodeToNetworkInit( ZFunction this, void* initArg ) {
    if( initArg ) {
        uint8_t mode = ( (uint8_t*)initArg )[0];
        CyclicBuffer buffer = &this->_tx_frame;
        switch( mode ) {
        case ZNET_ADD_NODE_STOP:
            buffer->_size = 7;
            if( ( buffer->_data = (uint8_t*)Heap_Alloc( buffer->_size ) ) == NULL )
                return ZNET_ERR_NOMEM;
            buffer->_data[4] = ADD_NODE_STOP;
            break;
        case ZNET_ADD_NODE_START:
            buffer->_size = 7;
            if( ( buffer->_data = (uint8_t*)Heap_Alloc( buffer->_size ) ) == NULL )
                return ZNET_ERR_NOMEM;
            buffer->_data[4] = ( ADD_NODE_ANY | ADD_NODE_OPTION_NETWORK_WIDE | ADD_NODE_OPTION_NORMAL_POWER );
            break;
        case ZNET_ADD_NODE_SMART_START:
            buffer->_size = 7;
            if( ( buffer->_data = (uint8_t*)Heap_Alloc( buffer->_size ) ) == NULL )
                return ZNET_ERR_NOMEM;
            buffer->_data[4] = ( ADD_NODE_SMART_START | ADD_NODE_OPTION_NETWORK_WIDE | ADD_NODE_OPTION_NORMAL_POWER );
            break;
        case ZNET_ADD_NODE_DSK:
            buffer->_size = 15;  // transport layer (6) + bMode (1) + pDSK (8)
            if( ( buffer->_data = (uint8_t*)Heap_Alloc( buffer->_size ) ) == NULL )
                return ZNET_ERR_NOMEM;
            buffer->_data[4] = ADD_NODE_HOME_ID;
            memcpy( &buffer->_data[6], &( (uint8_t*)initArg )[1], 8 );
            break;
        }
        buffer->_begin = 0;
        buffer->_len = buffer->_end = this->_tx_frame_initial_len = buffer->_size - 2;
        return ZNET_ERR_OK;
    }
    return ZNET_ERR_INVAL;
}

static FuncResults_e ICACHE_FLASH_ATTR __AddNodeToNetworkAck( ZFunction this ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ADD_NODE_SMART_START == ( buffer->_data[4] & ADD_NODE_SMART_START ) ) {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
        }
        return FuncResults_Done;
    }
    return FuncResults_Next;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcAddNodeToNetwork = {
    0x4a,                        // _id
    "AddNodeToNetwork",          // _name
    __AddNodeToNetworkInit,      // _Init
    __AddNodeToNetworkAck,       // _ProcessACK
    NULL,                        // _ProcessRESPONSE
    __AddNodeToNetworkCallback,  // _ProcessCALLBACK
    NULL                         // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_add_node_to_network( uint8_t mode,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcAddNodeToNetwork, &mode, Callback, callbackArg );
    if( func == NULL ) {
        return ZNET_ERR_NOMEM;
    }
    uint32_t timeout;
    switch( mode ) {
    case ZNET_ADD_NODE_STOP:
        ZFunction_Free_From_All_List( &fcAddNodeToNetwork );
        // No break, proceed to the timeout
    case ZNET_ADD_NODE_SMART_START:
        timeout = FUNCTION_CLASS_TIMEOUT_500_MS;
        break;
    default:
        timeout = FUNCTION_CLASS_TIMEOUT_1_MIN;
    }
    return ZNet_AddFuncToTX_timeout( func, timeout );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_add_node_dsk_to_network( const uint8_t* dsk,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg ) {
    uint8_t data[9];
    data[0] = ZNET_ADD_NODE_DSK;
    memcpy( &data[1], dsk, 8 );
    ZFunction func = ZFunction_Create( &fcAddNodeToNetwork, data, Callback, callbackArg );
    if( func == NULL ) {
        return ZNET_ERR_NOMEM;
    }
    return ZNet_AddFuncToTX_timeout( func, FUNCTION_CLASS_TIMEOUT_1_MIN );
}
