//
// Created by Fedorov on 18.02.21.
//

#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "ZPlatform.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"

typedef struct _NvmData {
    uint8_t operation;
    uint8_t _length;
    uint8_t _offsetM;
    uint8_t _offsetL;
    const uint8_t* _data;
} NvmData_t;
typedef NvmData_t* NvmData;

static FuncResults_e ICACHE_FLASH_ATTR __NvmBackupRestoreResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "NvmBackup\r\n" );
    //    print_hex_buf( data, size );

    if( this->_Callback ) {
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __NvmBackupRestoreInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( initArg == NULL )
        return ZNET_ERR_INVAL;
    uint8_t len_tx = 6;
    NvmData data = (NvmData)initArg;

    switch( data->operation ) {
    case NVM_OPERATION_OPEN:   // OPEN
    case NVM_OPERATION_CLOSE:  // CLOSE
        len_tx = 6;
        break;
    case NVM_OPERATION_READ:  // READ
        len_tx = 9;
        break;
    case NVM_OPERATION_WRITE:  // WRITE
        len_tx = 9 + data->_length;
        break;
    }
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( len_tx + 1 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_data[4] = data->operation;
    buffer->_size = len_tx;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = len_tx - 1;

    switch( data->operation ) {
    case NVM_OPERATION_OPEN:   // OPEN
    case NVM_OPERATION_CLOSE:  // CLOSE
        break;
    case NVM_OPERATION_READ:  // READ
        buffer->_data[5] = data->_length;
        buffer->_data[6] = data->_offsetM;
        buffer->_data[7] = data->_offsetL;
        break;
    case NVM_OPERATION_WRITE:  // WRITE
        buffer->_data[5] = data->_length;
        buffer->_data[6] = data->_offsetM;
        buffer->_data[7] = data->_offsetL;
        for( uint8_t i = 0; i < data->_length; i++ ) {
            buffer->_data[8 + i] = data->_data[i];
        }
        break;
    }

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcNvmBackupRestore = {
    0x2e,                        // _id
    "NvmBackupRestore",          //
    __NvmBackupRestoreInit,      // _Init
    NULL,                        // _ProcessACK
    __NvmBackupRestoreResponse,  // _ProcessRESPONSE
    NULL,                        // _ProcessCALLBACK
    NULL                         // _IsCallbackFrameMine
};

znet_error_t ICACHE_FLASH_ATTR znet_fc_nvm_backup_restore( uint8_t cmd,
                                                           uint8_t length,
                                                           uint8_t offsetM,
                                                           uint8_t offsetL,
                                                           const uint8_t* buff,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg ) {
    NvmData data = Heap_Alloc( sizeof( NvmData_t ) );

    data->_data = buff;
    data->_length = length;
    data->_offsetL = offsetL;
    data->_offsetM = offsetM;
    data->operation = cmd;
    ZFunction func = ZFunction_Create( &fcNvmBackupRestore, data, Callback, callbackArg );
    Heap_Free( data );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
