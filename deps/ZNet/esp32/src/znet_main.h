/**
 * @file znet_main.h
 * @date 19 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_MAIN_H
#define ZNET_MAIN_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <znet/znet.h>

extern const znet_callbacks_t* znet_cb;

////////////////////////////////////////////////////////////////////////

/// TODO: need refactoring!

#include "typedefs.h"

#include "ZPlatform.h"
#include "ZFunctionClass.h"

#include "znet.h"
#include "znet_lib.h"
#include "serial_port.h"
#include "typedefs.h"

/**
 * @brief znet_utils_get_security_encap_type_by_node_id_and_command
 *
 * @param node_id Node ID
 * @param command Command
 * @return Encapsulation type
 */
encap_type_t znet_utils_get_security_encap_type_by_node_id_and_command( znet_node_id_t node_id,
                                                                        znet_command_class_t command );

/**
 * @brief znet_utils_get_highest_security
 *
 * @param flags Node ID
 * @return highest security from flags
 */
uint8_t znet_utils_get_highest_security( uint8_t flags );

/**
 * @brief Returns a external code of lib's error
 *
 * @param Internal err reason.
 */
znet_error_t znet_get_error(
    ZFuncFailures_e reason );  // TODO  parameter reason  needs to use type ZFuncFailures_e  or _ZWError .....
// TODO it has be private in zlib or absent.  Remove after functions will have returned err instead bool

//////////////////////////////////////////////////////////////////////////////

extern uint8_t Z_WAVE_LIBRARY_TYPE;
extern uint8_t Z_WAVE_PROTOCOL_VERSION;
extern uint8_t Z_WAVE_PROTOCOL_SUB_VERSION;
extern bool forced_requests_timer_run;

#define FW_MAX_COUNT 16
#define FW_1 2
#define FW_2 74
#define HW_1 0x02  // z_wave
#ifdef ETHERNET
#define HW_2 0x02  // 2 with Ethernet 1 - without
#else
#define HW_2 0x01  // 2 with Ethernet 1 - without
#endif

#define MANUFACTURER_ID_1 0x02
#define MANUFACTURER_ID_2 0x5B
#define PRODUCT_TYPE_ID_1 0x00
#define PRODUCT_TYPE_ID_2 0x02
#define PRODUCT_ID_1 0x00
#define PRODUCT_ID_2 0x01
#define INST_ICON_TYPE_1 0x01
#define INST_ICON_TYPE_2 0x00
#define USER_ICON_TYPE_1 0x01
#define USER_ICON_TYPE_2 0x00
//////////////////////////////////////////////////////////////////////////////

extern ZNet_t znet_instance;
extern uint8_t learn_mode;
extern uint8_t SIS_id;

void ICACHE_FLASH_ATTR print_D_I_CC_event( const ZNet znet,
                                           ZWDeviceChangeType type,
                                           uint8_t node_id,
                                           uint8_t instance_id,
                                           uint8_t command_id,
                                           void* arg );
void ICACHE_FLASH_ATTR print_znet_terminated( const ZNet znet );

bool ICACHE_FLASH_ATTR znet_lib_init( void );

void ICACHE_FLASH_ATTR znet_buttons_processing( uint8_t id, uint8_t state, uint8_t wu );

#endif  // ZNET_MAIN_H
