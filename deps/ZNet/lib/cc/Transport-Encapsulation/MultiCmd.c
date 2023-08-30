#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "znet_log.h"
#include "znet_main.h"

#include "SendData.h"

#include <string.h>

typedef struct {
    uint8_t length;
    uint8_t data[];
} ZW_MULTI_CMD_ENCAPSULATED_COMMAND;

typedef struct {
    uint8_t command_class_id;
    uint8_t command_id;
    uint8_t commands_count;
    ZW_MULTI_CMD_ENCAPSULATED_COMMAND bundle[];
} ZW_MULTI_CMD_ENCAP_FRAME;

bool send_data_context_bundle_init( uint8_t commands_count ) {
    send_data_context.commandIdx = 0;
    send_data_context.bundle = malloc( sizeof( BundledCommand_t ) * commands_count );

    if( send_data_context.bundle == NULL ) {
        ZNET_LOGE( "send_data_context_bundle_init: malloc failed\n" );
        return false;
    }

    return true;
}

void send_data_context_bundle_deinit() {
    if( send_data_context.bundle == NULL )
        return;

    for( uint8_t i = 0; i != send_data_context.commandIdx; ++i )
        free( (void*)send_data_context.bundle[i].command );

    free( send_data_context.bundle );
    send_data_context.bundle = 0;

    send_data_context.commandIdx = 0;
}

uint8_t get_total_bundle_length() {
    uint8_t encap_length = 0;

    // length of command + 1 byte for the length itself
    for( uint8_t i = 0; i != send_data_context.commandIdx; ++i )
        encap_length += send_data_context.bundle[i].length + 1;

    return encap_length;
}

ZW_MULTI_CMD_ENCAP_FRAME* fill_response_frame( uint8_t* data ) {
    ZW_MULTI_CMD_ENCAP_FRAME* frame = (ZW_MULTI_CMD_ENCAP_FRAME*)data;

    frame->command_class_id = COMMAND_CLASS_MULTI_CMD;
    frame->command_id = MULTI_CMD_ENCAP;

    frame->commands_count = send_data_context.commandIdx;

    ZW_MULTI_CMD_ENCAPSULATED_COMMAND* command = frame->bundle;
    for( uint8_t i = 0; i != send_data_context.commandIdx; ++i ) {
        command->length = send_data_context.bundle[i].length;
        memcpy( command->data, send_data_context.bundle[i].command, command->length );
        command += command->length + 1;
    }

    return frame;
}

void ICACHE_FLASH_ATTR znet_cc_multi_cmd_receive( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    const ZW_MULTI_CMD_ENCAP_FRAME* frame = (const ZW_MULTI_CMD_ENCAP_FRAME*)( cc_data );

    const bool cc_supported =
        znet_cb->check_node_supports_command_class( node_id, func->_endpoint, COMMAND_CLASS_MULTI_CMD, znet_cb->arg );

    const bool initialized = cc_supported && send_data_context_bundle_init( frame->commands_count );

    const ZW_MULTI_CMD_ENCAPSULATED_COMMAND* command = frame->bundle;
    while( (uint8_t*)command < cc_data + cc_data_len ) {
        ZNet_ApplicationCommandHandler( command->data, command->length, node_id, func->_security, func->_endpoint );
        command += command->length + 1;
    }

    if( !initialized )
        return;

    const uint8_t encap_length = get_total_bundle_length();
    uint8_t response_data[encap_length + 3];
    ZW_MULTI_CMD_ENCAP_FRAME* response_frame = fill_response_frame( response_data );

    send_data_context_bundle_deinit();

    // Supporting nodes SHOULD NOT return an answer Multi Command encapsulated if returning a single command
    if( response_frame->commands_count > 1 ) {
        znet_fc_send_data(
            node_id, func->_endpoint, sizeof( response_data ), response_data, NULL, NULL, func->_security );
    }
    else if( response_frame->commands_count == 1 ) {
        znet_fc_send_data( node_id,
                           func->_endpoint,
                           response_frame->bundle->length,
                           response_frame->bundle->data,
                           NULL,
                           NULL,
                           func->_security );
    }
}
