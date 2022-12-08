#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "ZPlatform.h"

#include "znet_fc_private.h"

#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

/*
//----------------------------------------------------------------------------
static
FuncResults_e
ICACHE_FLASH_ATTR
__SetDefaultCallback( ZFunction this )
{
    //if( job->_callback_id ) {
    //    ZControllerData_SetLastIncludedDevice( &znet->_controllerData, -1 );
    //    ZControllerData_SetLastExcludedDevice( &znet->_controllerData, -1 );
    //    ///Log_Write( "Done");
    //    ZJob_OnSuccess( job );
    //}
    //ZJob_Remove( job );

    if( this->_Callback ) (*this->_Callback)( this, this->_callbackArg, FUNC_OK );

        return FuncResults_Done;
}
*/

/*
//----------------------------------------------------------------------------
        //{ 0x01, 0x03, 0x00, 0x08 };
static
bool
ICACHE_FLASH_ATTR
__SetDefaultInit( ZFunction this, void* UNUSED(initArg) )
{
        CyclicBuffer buffer = &this->_tx_frame;

        if( (buffer->_data = (uint8_t*)Heap_Alloc( 7 )) == NULL ) return ZNET_ERR_NOMEM;

        buffer->_size  = 7;
        buffer->_begin = 0;
        buffer->_len = buffer->_end = this->_tx_frame_initial_len =  5;

        buffer->_data[ 4 ] = 0;

        return ZNET_ERR_OK;
}
*/

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SetDefaultCALLBACK( ZFunction this ) {
    if( this->_Callback )
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
//{ 0x01, 0x03, 0x00, 0x08 };
static znet_error_t ICACHE_FLASH_ATTR __SetDefaultInit( ZFunction this, void* UNUSED( initArg ) ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 6 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 6;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 4;

    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    // data[ 4 ] = 0xFF; // callback_id
    // data[ 5 ] = 0xFF; // crc

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSetDefault = {
    0x42,              // _id
    "SetDefault",      // _name
    __SetDefaultInit,  // _Init
    NULL,              // _ProcessACK
    NULL,              // _ProcessRESPONSE
    // __SetDefaultCallback, // _ProcessCALLBACK
    __SetDefaultCALLBACK,  // _ProcessCALLBACK
    // NULL,                 // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
    /*
        "SetDefault",
        "Reset the cntrlr",
        NULL,                               // response handler
        __SetDefaultCallback,               // callback handler
        NULL,                               // ack handler
        NULL,                               // timeout handler
        //FALSE,                              // ugent
        //TRUE,                               // block
        //FALSE,                              // allow duplicate
        //FALSE,                              // sleeping check
        //FALSE,                              // security
        //FALSE,                              // has node
        //FALSE,                              // send to node
        //0.0f,
        //60.0f,                              // extra callback timeout
        //0.0f
    */
};

/*
//----------------------------------------------------------------------------
ZWError
ICACHE_FLASH_ATTR
znet_fc_set_default( ZFunction_Callback Callback, void* callbackArg )
{
    if( znet == NULL ) return InvalidArg;

    if( ! ZNet_IsFCSupported( znet, fcSetDefault._id ) ) return NotSupported;

    ZJob job = ZJob_Create( znet, &fcSetDefault, 0, NULL, successCallback,
failureCallback, NULL ); if( job == NULL ) return BadAllocation; return
ZNet_AddJob( znet, job );
}
*/
//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_default( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSetDefault, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
