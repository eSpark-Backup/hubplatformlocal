#include "typedefs.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//----------------------------------------------------------------------------
// Command Class User Code //

// User Code Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: user_id
// User Identifier
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_get( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t user_id,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_USER_CODE, USER_CODE_GET, user_id };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// User Code Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: user_id
// User Identifier
//
// @param: user_id_status
// User ID Status
//
// @param: user_code_size
// Size user_code
//
// @param: user_code_size
// user_code
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_set( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t user_id,
                                                      uint8_t user_id_status,
                                                      uint8_t user_code_size,
                                                      const uint8_t* user_code,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type ) {
    uint8_t header[] = { COMMAND_CLASS_USER_CODE, USER_CODE_SET, user_id, user_id_status };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + user_code_size;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return FALSE;

    memcpy( &data[0], header, header_len );
    if( user_code_size > 0 )
        memcpy( &data[header_len], user_code, user_code_size );

    bool result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Users Number Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_users_number_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_USER_CODE, USERS_NUMBER_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Users Code Checksum Report
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_checksum_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_USER_CODE, USER_CODE_CHECKSUM_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Master Code Report
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_master_code_set( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t master_code_length,
                                                        const uint8_t* master_code,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type ) {
    uint8_t data[3 + master_code_length];
    data[0] = COMMAND_CLASS_USER_CODE;
    data[1] = MASTER_CODE_SET;
    data[2] = master_code_length;
    memcpy( data + 3, master_code, master_code_length );

    return znet_fc_send_data( node_id, channel_id, 3 + master_code_length, data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_master_code_get( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type ) {
    uint8_t data[] = {
        COMMAND_CLASS_USER_CODE,
        MASTER_CODE_GET,
    };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Extended User Code Report
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_extended_user_code_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t user_id,
                                                               bool should_report_more,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[] = {
        COMMAND_CLASS_USER_CODE, EXTENDED_USER_CODE_GET, user_id >> 8, user_id & 0xFF, should_report_more ? 0xFF : 0
    };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_extended_user_code_set( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint8_t user_codes_number,
                                                               uint8_t extended_user_codes_size,
                                                               const uint8_t* extended_user_codes,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[3 + extended_user_codes_size];
    data[0] = COMMAND_CLASS_USER_CODE;
    data[1] = EXTENDED_USER_CODE_SET;
    data[2] = user_codes_number;
    memcpy( data + 3, extended_user_codes, extended_user_codes_size );
    return znet_fc_send_data(
        node_id, channel_id, 3 + extended_user_codes_size, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Keypad Mode
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_keypad_mode_set( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t keypad_mode,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_USER_CODE, USER_CODE_KEYPAD_MODE_SET, keypad_mode };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_keypad_mode_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_USER_CODE, USER_CODE_KEYPAD_MODE_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// User Code Capabilities
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_capabilities_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_USER_CODE, USER_CODE_CAPABILITIES_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
