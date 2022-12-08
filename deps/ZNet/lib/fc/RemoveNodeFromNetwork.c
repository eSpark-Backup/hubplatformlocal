#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"

#define REMOVE_NODE_ANY 0x01   // initiate removal of node of any type
#define REMOVE_NODE_STOP 0x05  // abort the removal process

#define REMOVE_NODE_STATUS_LEARN_READY 0x01
#define REMOVE_NODE_STATUS_NODE_FOUND 0x02
#define REMOVE_NODE_STATUS_REMOVING_SLAVE 0x03
#define REMOVE_NODE_STATUS_REMOVING_CONTROLLER 0x04
#define REMOVE_NODE_STATUS_DONE 0x06
#define REMOVE_NODE_STATUS_FAILED 0x07
#define ADD_NODE_STATUS_NOT_PRIMARY 0x23

extern void ICACHE_FLASH_ATTR RemNodeInvokedCallback();

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RemoveNodeFromNetworkCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "RemoveNodeFromNetwork\r\n" );

    if( size < 4 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[3] ) {
    case REMOVE_NODE_STATUS_LEARN_READY:
        Log_Write( "Remove node: Push a button on the device to be removed\r\n" );
        RemNodeInvokedCallback();
        return FuncResults_Repeat;

    case REMOVE_NODE_STATUS_NODE_FOUND:
        Log_Write( "Remove node: Node found:%d\r\n", data[4] );
        if( size < 5 ) {
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        return FuncResults_Repeat;

    case REMOVE_NODE_STATUS_REMOVING_SLAVE:
        Log_Write( "Remove node: Removing slave:%d\r\n", data[4] );
        if( size < 5 ) {
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        this->_dataSize = size - 2;
        if( ( this->_dataPtr = (uint8_t*)Heap_Alloc( this->_dataSize ) ) != NULL ) {
            memcpy( this->_dataPtr, &data[2], this->_dataSize );
        }
        return FuncResults_Repeat;

    case REMOVE_NODE_STATUS_REMOVING_CONTROLLER:
        Log_Write( "Remove node: Removing controller:%d\r\n", data[4] );
        if( size < 5 ) {
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        this->_dataSize = size - 2;
        if( ( this->_dataPtr = (uint8_t*)Heap_Alloc( this->_dataSize ) ) != NULL ) {
            memcpy( this->_dataPtr, &data[2], this->_dataSize );
        }
        return FuncResults_Repeat;

    case REMOVE_NODE_STATUS_DONE:
        Log_Write( "Remove node: Done this->_Callback-%08p\r\n", this->_Callback );

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
        return FuncResults_Done;
    case REMOVE_NODE_STATUS_FAILED:
        Log_Write( "Remove node: Failed\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
        return FuncResults_Done;
    case ADD_NODE_STATUS_NOT_PRIMARY:
        Log_Write( "Remove node: Not primary cntrlr. Failed\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
        return FuncResults_Done;
    default:
        Log_Write( "Remove node: Invalid status: %02x\r\n", data[3] );
        return FuncResults_Restart;
    }
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __RemoveNodeFromNetworkInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 7 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 7;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 5;

    buffer->_data[4] =
        ADD_NODE_OPTION_NORMAL_POWER | ADD_NODE_OPTION_NETWORK_WIDE |
        ( ( initArg ) ? ( ( *(bool*)initArg ) ? REMOVE_NODE_ANY : REMOVE_NODE_STOP ) : REMOVE_NODE_STOP );
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcRemoveNodeFromNetwork = {
    0x4b,                             // _id
    "RemoveNodeFromNetwork",          //
    __RemoveNodeFromNetworkInit,      // _Init
    NULL,                             // _ProcessACK
    NULL,                             // _ProcessRESPONSE
    __RemoveNodeFromNetworkCallback,  // _ProcessCALLBACK
    NULL                              // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckFuncToArg( const void* object, const void* arg ) {
    ZFunction func = (ZFunction)object;
    ZFunction currentFunc = (ZFunction)arg;
    return ( currentFunc->_fc->_id == func->_fc->_id && currentFunc->_node_id == func->_node_id );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_remove_node_from_network( bool startStop,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcRemoveNodeFromNetwork, &startStop, Callback, callbackArg );
    if( func == NULL ) {
        return ZNET_ERR_NOMEM;
    }
    uint32_t timeout = FUNCTION_CLASS_TIMEOUT_500_MS;
    if( startStop ) {
        timeout = FUNCTION_CLASS_TIMEOUT_1_MIN;
        List_Entry entry = ZFunction_Find_Function_in_All_List_By_Init_Arg( &CheckFuncToArg, (void*)func );
        if( entry ) {
            return ZNET_ERR_BUSY;
        }
    }
    ZFunction_Free_From_All_List( &fcRemoveNodeFromNetwork );
    return ZNet_AddFuncToTX_timeout( func, timeout );
}
