#include "znet/znet.h"
#include "lib/znet_lib_fc.h"
#include "znet_main.h"
#include "znet_log.h"

typedef struct result_frame {
    uint8_t return_value;
    uint8_t length;
    uint8_t offset_msb;
    uint8_t offset_lsb;
    uint8_t buffer[];
} result_frame;

typedef union rw_param {
    void* data;
    uint16_t offset;
} rw_param;

static znet_error_t get_backup_error( ZFuncFailures_e reason ) {
    return reason == FUNC_OK ? ZNET_ERR_OK : znet_get_error( reason );
}

static void _znet_open_backup_session_cb( ZFunction func,
                                          void* arg,
                                          ZFuncFailures_e reason,
                                          const void* dataPtr,
                                          size_t dataSize ) {
    const result_frame* frame = (const result_frame*)( dataPtr );

    if( znet_cb->backup_session_open_result ) {
        if( frame ) {
            znet_cb->backup_session_open_result( get_backup_error( reason ),
                                                 frame->return_value,
                                                 frame->offset_lsb | ( frame->offset_msb << 8 ),
                                                 znet_cb->arg );
        }
        else {
            znet_cb->backup_session_open_result( get_backup_error( reason ), 0xFF, 0, znet_cb->arg );
        }
    }
}

static void _znet_close_backup_session_cb( ZFunction func,
                                           void* arg,
                                           ZFuncFailures_e reason,
                                           const void* dataPtr,
                                           size_t dataSize ) {
    const result_frame* frame = (const result_frame*)( dataPtr );

    if( znet_cb->backup_session_close_result ) {
        znet_cb->backup_session_close_result(
            get_backup_error( reason ), frame ? frame->return_value : 0xFF, znet_cb->arg );
    }
}

static void _znet_write_backup_data_cb( ZFunction func,
                                        void* arg,
                                        ZFuncFailures_e reason,
                                        const void* dataPtr,
                                        size_t dataSize ) {
    const result_frame* frame = (const result_frame*)( dataPtr );

    if( znet_cb->backup_write_result ) {
        rw_param param;
        param.data = arg;

        if( frame ) {
            znet_cb->backup_write_result( get_backup_error( reason ),
                                          param.offset,
                                          frame->return_value,
                                          ( uint16_t )( frame->offset_lsb | ( frame->offset_msb << 8 ) ),
                                          frame->length,
                                          znet_cb->arg );
        }
        else {
            znet_cb->backup_write_result( get_backup_error( reason ), param.offset, 0xFF, 0, 0, znet_cb->arg );
        }
    }
}

static void _znet_read_backup_data_cb( ZFunction func,
                                       void* arg,
                                       ZFuncFailures_e reason,
                                       const void* dataPtr,
                                       size_t dataSize ) {
    const result_frame* frame = (const result_frame*)( dataPtr );

    if( znet_cb->backup_read_result ) {
        rw_param param;
        param.data = arg;

        if( frame ) {
            znet_cb->backup_read_result( get_backup_error( reason ),
                                         param.offset,
                                         frame->return_value,
                                         ( uint16_t )( frame->offset_lsb | ( frame->offset_msb << 8 ) ),
                                         frame->length,
                                         frame->buffer,
                                         znet_cb->arg );
        }
        else {
            znet_cb->backup_read_result( znet_get_error( reason ), param.offset, 0xFF, 0, 0, NULL, znet_cb->arg );
        }
    }
}

znet_error_t znet_open_backup_session() {
    ZNET_LOGI( "znet_open_backup_session\n" );
    return znet_fc_nvm_backup_restore( NVM_OPERATION_OPEN, 0, 0, 0, NULL, _znet_open_backup_session_cb, NULL );
}

znet_error_t znet_write_backup_data( uint16_t offset, uint8_t dataLength, const uint8_t* data ) {
    rw_param param;
    param.data = NULL;
    param.offset = offset;
    return znet_fc_nvm_backup_restore(
        NVM_OPERATION_WRITE, dataLength, offset >> 8, offset & 0xFF, data, _znet_write_backup_data_cb, param.data );
}

znet_error_t znet_read_backup_data( uint16_t offset, uint8_t dataLength ) {
    rw_param param;
    param.data = NULL;
    param.offset = offset;

    return znet_fc_nvm_backup_restore(
        NVM_OPERATION_READ, dataLength, offset >> 8, offset & 0xFF, NULL, _znet_read_backup_data_cb, param.data );
}

znet_error_t znet_close_backup_session() {
    return znet_fc_nvm_backup_restore( NVM_OPERATION_CLOSE, 0, 0, 0, NULL, _znet_close_backup_session_cb, NULL );
}
