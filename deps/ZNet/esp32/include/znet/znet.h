/**
 * @file znet.h
 * @date 9 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_H
#define ZNET_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "znet_defs.h"
#include "znet_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Home ID
 */
typedef uint32_t znet_home_id_t;

/**
 * @brief Node ID
 *
 * Valid values 1..232
 * Specific values:
 * 0    - any
 * 0xFF - invalid
 */
typedef uint8_t znet_node_id_t;

#define ZNET_NODE_ID_ANY 0
#define ZNET_NODE_ID_MIN 1
#define ZNET_NODE_ID_MAX 232
#define ZNET_NODE_ID_INVALID 0xFF

/**
 * @brief Channel ID
 */
typedef uint8_t znet_node_channel_id_t;
#define ZNET_CHANNEL_ID_ROOT 0
#define ZNET_CHANNEL_ID_MIN 1
#define ZNET_CHANNEL_ID_MAX 127
#define ZNET_CHANNEL_ID_INVALID 255

/**
 * @brief Endpoint full address
 */
typedef struct znet_endpoint_t {
    znet_node_id_t node_id;
    znet_node_channel_id_t channel_id;
} znet_endpoint_t;

/**
 * @brief Basic class
 */
typedef uint8_t znet_basic_class_t;

/**
 * @brief Generic class
 */
typedef uint8_t znet_generic_class_t;

/**
 * @brief Specific class
 */
typedef uint8_t znet_specific_class_t;

/**
 * @brief Command class
 */
typedef uint8_t znet_command_class_t;

/**
 * @brief Command class version
 */
typedef uint8_t znet_command_class_version_t;

#define ZNET_COMMAND_CLASS_VERSION_NONE 0
#define ZNET_COMMAND_CLASS_VERSION_MIN 1
#define ZNET_COMMAND_CLASS_VERSION_MAX 255

typedef union {
    void* data;
    uint8_t nhid[4];
} znet_smart_start_data_t;

/**
 * @brief TBD.
 */
typedef struct znet_nodeinfo_t {
    znet_node_id_t node_id;                   /**< Node ID */
    uint8_t capability;                       /**< Capability */
    uint8_t security;                         /**< Security */
    znet_basic_class_t basic;                 /**< Basic class type */
    znet_generic_class_t generic;             /**< Generic class type */
    znet_specific_class_t specific;           /**< Specific class type */
    uint8_t commands_count;                   /**< Commands classes count */
    uint8_t flags;                            /**< Flags */
    znet_smart_start_data_t smart_start_data; /**< Additional data for Smart Start devices. */
    znet_command_class_t commands[];          /**< Commands classes, max 35 */
} znet_nodeinfo_t;

#define ZNET_NODEINFO_CAPABILITY_LISTENING \
    0x80 /**< Node is always listening (typically AC powered nodes) and stationary */

#define ZNET_NODEINFO_SECURITY_MASK 0x07

#define ZNET_NODEINFO_SECURITY_NONE 0x00
#define ZNET_NODEINFO_SECURITY_S2_UNAUTH 0x01
#define ZNET_NODEINFO_SECURITY_S2_AUTH 0x02
#define ZNET_NODEINFO_SECURITY_S2_ACCESS 0x03
#define ZNET_NODEINFO_SECURITY_S0 0x04
#define ZNET_NODEINFO_SECURITY_UNKNOWN 0xFF

#define ZNET_NODEINFO_SECURITY_FLIRS_250 \
    0x20 /**< Node should act as a Frequently Listening Routing Slave with a wakeup interval of 250ms */
#define ZNET_NODEINFO_SECURITY_FLIRS_1000 \
    0x40 /**< node should act as a Frequently Listening Routing Slave with a wakeup interval of 1000ms */
#define ZNET_NODEINFO_SECURITY_OPT_FUNC 0x80 /**< Node supports other command classes than the ones mandatory */

/// TODO: flags must be move to internal API after refactoring storage
#define ZNET_NODEINFO_FLAGS_REPLACED 0x01
#define ZMET_NODEINFO_FLAGS_GETPROTOINFO 0x02

typedef struct inclusion_controller_info_t {
    znet_node_id_t id;
    uint8_t security;
} inclusion_controller_info_t;

/**
 * @brief Function prototype for allocate/reallocate/freed block memmory
 *
 * The allocator function must provide a functionality similar to realloc, but
 * not exactly the same.
 * - ptr is NULL if allocate new block, the allocator behaves like malloc
 * - when size is zero, the allocator must return NULL
 * - if ptr is not NULL and size is zero, the allocator behaves like free
 * - when size is not zero and ptr is not NULL, the allocator behaves like
 * realloc
 * - when size is not zero, the allocator returns NULL if and only if it cannot
 * fill the request
 * - znet assumes that the allocator never fails when old size for realloc <=
 * size
 *
 * Example of implementation:
 * @code
 * static void* znet_alloc( void* ptr, size_t size, void* arg)
 * {
 *     if( size != 0 )
 *         return realloc( ptr, size );
 *
 *     free( ptr );
 *     return NULL;
 * }
 * @endcode
 *
 * @param ptr A pointer to the block being allocated/reallocated/freed
 * @param size The new size of the block
 * @param arg Parameter for callback functions
 * @return pointer to allocated block
 */
typedef void* ( *ZNET_ALLOC )( void* ptr, size_t size, void* arg );

/**
 * @brief Function prototype for clock that cannot be set and represents
 * monotonic time since some unspecified starting point.
 *
 * @param arg Parameter for callback functions
 * @return Time from monotonic timer in ms. On error, 0 is returned
 */
typedef uint64_t ( *ZNET_CLOCK )( void* arg );

/**
 * @brief Function prototype for log massages
 *
 * Support 4 levels:
 * 1 - error
 * 2 - warning
 * 3 - info
 * 4 - debug
 *
 * @param lvl Message level
 * @param fmt Format string that follows the same specifications as format in
 * printf
 * @param list A value identifying a variable arguments list initialized with
 * va_start
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_LOG )( int lvl, const char* fmt, va_list list, void* arg );

/**
 * @brief Write data to uart (to zwave module connected to uart)
 *
 * Use cases:
 * 1. Get free space in uart tx buffer - data == NULL, size == 0,
 * ret_size = <ptr>
 * 2. Write all data (limit by size) to uart tx buffer - data = <ptr>,
 * size != 0, ret_size = <ptr> For write call is blocked if all data can
 * not be written to uart buffer
 *
 * @param data Pointer to buffer with data
 * @param size The size of the data for write
 * @param ret_size Return free size in uart buffer or param size (see use cases)
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_UART_WRITE )( const void* data, size_t size, size_t* ret_size, void* arg );

/**
 * @brief Read data from uart (from zwave module connected to uart)
 *
 * Use cases:
 * 1. Get free space in uart rx buffer - data == NULL, size == 0,
 * ret_size = <ptr>
 * 2. Read all data (limit by size) from uart rx buffer - data = <ptr>,
 * size !=  0, ret_size = <ptr> For read call is blocked if all data can
 * not be readed from uart buffer
 *
 * @param data Pointer to buffer for data
 * @param size The size of the data for read
 * @param ret_size Return size of available data in uart buffer or param size
 * (see use cases)
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_UART_READ )( void* data, size_t size, size_t* ret_size, void* arg );

/**
 * @brief Reset zwave module and uart connected to it
 *
 * Implement if it is possible to hard reset the module
 *
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_UART_RESET )( void* arg );

/**
 * @brief Function prototype for writing data to storage
 *
 * @param offset Offset in storage for write
 * @param data Pointer to buffer with data
 * @param size The size of the data for write
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_STORE_SAVE )( size_t offset, const void* data, size_t size, void* arg );

/**
 * @brief Function prototype for reading data from storage
 *
 * If read outside of the current stored data, then zero out the data and return
 * success.
 *
 * @param offset Offset in storage for read
 * @param data Pointer to buffer for data
 * @param size The size of the data for read
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_STORE_LOAD )( size_t offset, void* data, size_t size, void* arg );

/**
 * @brief Function prototype for reset storage
 *
 * If reserv is not 0 to reserve space for data
 *
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_STORE_RESET )( void* arg );

/**
 * @brief Function prototype for notify: set default complated
 *
 */
typedef void ( *ZNET_INIT_FINISH )( znet_error_t err, void* arg );

/**
 * @brief Function prototype for notify: set default complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_SET_DEFAULT_RESULT )( znet_error_t err, void* arg );

/**
 * @brief Function prototype for notify: set default started
 *
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_SET_DEFAULT_START )( void* arg );

/**
 * @brief Function prototype for notify: network update complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NETWORK_UPDATE_RESULT )( znet_error_t err, void* arg );

/**
 * @brief Function prototype for notify: network update complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Return node_id
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_REQUEST_NEIGHBOR_RESULT )( znet_error_t err, znet_node_id_t node_id, void* arg );

/**
 * @brief Function prototype for notify: add node complated
 *
 * If ( ( node_info->security & ZNET_NODEINFO_SECURITY_MASK ) ==
 * ZNET_NODEINFO_SECURITY_S0 ) then please wait node_update_info_result &&
 * node_cmd_security_supported_result notifications
 *
 * If the node supports COMMAND_CLASS_WAKEUP, you should always call "znet_node_cmd_wakeup_sleep" command at the end of
 * the interview
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_info Return node info
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_ADD_RESULT )( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg );

/**
 * @brief Function prototype for notify: add node security failed.
 *
 * Sent if the inclusion was aborted during s2 bootstrapping by some reason.
 * It may happen e.g. if a Smart Start node was requested to connect in unsecured mode. In such case the inclusion
 * process cannot continue.
 *
 * @param node_id The ID of a node that aborted inclusion.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_ADD_SECURITY_FAILED )( znet_node_id_t node_id, void* arg );

/**
 * @brief Function prototype for notify: add node invoked
 *
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_ADD_INVOKED )( void* arg );

/**
 * @brief Function prototype for notify: remove node complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_info Return node info
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_REM_RESULT )( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg );

/**
 * @brief Function prototype for notify: remove node invoked
 *
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_REM_INVOKED )( void* arg );

/**
 * @brief Function prototype for notify: replace node complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_info Return node info
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_REPLACE_IF_FAILED_RESULT )( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg );

/**
 * @brief Function prototype for notify: learn node complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_info Return node info
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_LEARN_RESULT )( znet_error_t err, znet_node_id_t dest_node_id, void* arg );

/**
 * @brief Function prototype for notify: learn mode invoked
 *
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_LEARN_INVOKED )( void* arg );

/**
 * @brief Function prototype for notify: list node found node
 *
 * It is called many times. One time on the found node. node_info is NULL if
 * there are no more nodes.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_info Return node info
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_LIST_RESULT )( znet_error_t err, const uint8_t* nodes_list, void* arg );

/**
 * @brief Function prototype for notify: update node info
 *
 * From ApplicationControllerUpdate
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_UPDATE_INFO_RESULT )( znet_error_t err,
                                                znet_node_id_t node_id,
                                                const znet_nodeinfo_t* node_info,
                                                void* arg );

/** @brief Function prototype for notify: forced request timer on success
 *
 * It is called many times on repeat calls
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_FORCED_REQUEST_RESULT )( znet_error_t err, void* arg );

/**
 * @brief
 */
typedef uint8_t znet_S2_keys_t;

#define ZNET_SECURITY2_S2_CLASS_0 0x01  // Unauthenticated
#define ZNET_SECURITY2_S2_CLASS_1 0x02  // Authenticated
#define ZNET_SECURITY2_S2_CLASS_2 0x04  // Access
#define ZNET_SECURITY2_S2_CSA 0x40
#define ZNET_SECURITY2_S0_NET_KEY 0x80

/**
 * @brief Function prototype for select supported keys for S2 inclusion
 *
 * @param key_list Keys (bitmask)
 */
typedef void ( *ZNET_NODE_S2_SELECT_KEYS )( znet_S2_keys_t key_list, znet_node_id_t node_id, void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_S2_pub_key_t {
    uint8_t size;  /**< Key size */
    uint8_t key[]; /**< Key */
} znet_S2_pub_key_t;

/**
 * @brief Function prototype for request public key for S2 inclusion
 *
 * @param key_list Keys (bitmask)
 * @param key Remote node public key (part)
 */
typedef void ( *ZNET_NODE_S2_REQUEST_PUB_KEY )( uint8_t node_id,
                                                znet_S2_keys_t key_list,
                                                const znet_S2_pub_key_t* key,
                                                void* arg );

/**
 * @brief Function prototype for show public key for S2 inclusion
 *
 * @param key_list Keys (bitmask)
 * @param key Remote node public key (part)
 */
typedef void ( *ZNET_NODE_S2_SHOW_PUB_KEY )( znet_S2_keys_t key_list, const znet_S2_pub_key_t* key, void* arg );

/**
 * @brief Inform that a Smart Start node with the specified characteristics has been powered up.
 *
 * According to Z-Wave-Appl-Programmers-Guide 4.4.2.4 the length of NHID is 4 bytes.
 *
 * @param nodeId The updated node’s node ID (1..232).
 * @param nhid Pointer to a buffer that contains a Smart Start DSK derived HomeID of an already included Smart Start
 *             node (4 bytes long).
 * @param foreign Indicates whether the INIF has been received from a foreign network.
 * @param arg znet_callbacks_t::arg member value.
 */
typedef void ( *ZNET_NODE_INIF_RECEIVED )( uint8_t nodeId, uint8_t* nhid, bool foreign, void* arg );

/**
 * @brief Inform that a Smart Start node with the specified characteristics has requested inclusion.
 *
 * According to Z-Wave-Appl-Programmers-Guide 4.4.2.4 the length of NHID is 4 bytes.
 *
 * @param nhid Pointer to a buffer containing the Smart Start DSK derived HomeId of the Smart Start node wanting to be
 *             included.
 * @param arg znet_callbacks_t::arg member value.
 */
typedef void ( *ZNET_NODE_NHID_RECEIVED )( uint8_t* nhid, void* arg );

/**
 * @brief Reset zwave chip
 */
typedef void ( *RESET_ZWAVE_CHIP )( void* arg );

/**
 * @brief Function prototype for notify: sis updated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param sis_id Return sis_id
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_SIS_UPDATE )( znet_error_t err, znet_node_id_t node_id, void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_library_info_t {
    uint8_t size;   /**< Key size */
    uint8_t name[]; /**< Key */
} znet_library_info_t;

/**
 * @brief Function prototype for notify: sis updated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param sis_id Return sis_id
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_LIBRARY_INFO )( znet_error_t err, znet_library_info_t* info, uint8_t lib_type, void* arg );

/**
 * @brief Function prototype to check whether a node supports given command class
 *
 * @param node_id Node ID
 * @param endpoint Channel ID
 * @param command_class_id Command class ID
 * @param arg Parameter for callback functions
 */
typedef bool ( *ZNET_CHECK_NODE_SUPPORTS_COMMAND_CLASS )( znet_node_id_t node_id,
                                                          znet_node_channel_id_t endpoint,
                                                          znet_command_class_t command_class_id,
                                                          void* arg );

/**
 * @brief TBD.
 */
typedef struct {
    uint8_t ver;
    uint8_t sub_ver;
} znet_version_minor_major_t;

typedef struct znet_version_report_t {
    uint8_t lib_type;                   /**< Z-Wave Library Type */
    znet_version_minor_major_t proto;   /**< Z-Wave Protocol Version */
    uint8_t hard_ver;                   /**< Hardware version */
    uint8_t firms_count;                /**< Firmware count without firm_0 */
    znet_version_minor_major_t firms[]; /**< Firmware 0..N Version & Sub Version */
} znet_version_report_t;

/**
 * @brief Function prototype for notify: get version
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_VERSION_RESULT )( znet_error_t err,
                                                znet_node_id_t node_id,
                                                const znet_version_report_t* value,
                                                void* arg );

/**
 * @brief Function prototype for notify: request version
 *
 * @param data Current data
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_VERSION_FILL )( znet_version_report_t* data, void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_command_version_report_t {
    uint8_t __ver;                        /**< reserved */
    znet_command_class_t command;         /**< Command class */
    znet_command_class_version_t version; /**< Command class version */
} znet_command_version_report_t;

/**
 * @brief Function prototype for notify: get command class version
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_COMMAND_VERSION_RESULT )( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_command_class_t class_id,
                                                        znet_command_class_version_t version,
                                                        void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_version_capabilities_report_t {
    znet_command_class_t command; /**< Command class */
    bool version_supported;       /**< Version Get supported */
    bool command_class_supported; /**< Command Class Get supported */
    bool software_supported;      /**< Software Get supported */
} znet_version_capabilities_report_t;

/**
 * @brief Function prototype for notify: get version capabilities
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Capabilities
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_VERSION_CAPABILITIES_RESULT )( znet_error_t err,
                                                             znet_node_id_t node_id,
                                                             const znet_version_capabilities_report_t* value,
                                                             void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_version_t {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} znet_version_t;

typedef struct znet_software_version_report_t {
    znet_command_class_t command;                     /**< Command class */
    znet_version_t sdk_version;                       /**< SDK version */
    znet_version_t application_framework_api_version; /**< Z-Wave Application Framework API version */
    znet_version_t host_interface_version;            /**< Serial API Version */
    znet_version_t protocol_version;                  /**< Z-Wave protocol version */
    znet_version_t application_version;               /**< Application version */
    uint16_t application_framework_api_build_number;  /**< Z-Wave Application Framework build number */
    uint16_t host_interface_build_number;             /**< Serial API software build number */
    uint16_t protocol_build_number;                   /**< Z-Wave protocol build number */
    uint16_t application_build_number;                /**< Application software build number */
} znet_software_version_report_t;

/**
 * @brief Function prototype for notify: get software version information
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Software version information
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_SOFTWARE_VERSION_RESULT )( znet_error_t err,
                                                         znet_node_id_t node_id,
                                                         const znet_software_version_report_t* value,
                                                         void* arg );

/**
 * @brief Manufacturer specific
 */
typedef struct znet_manufacturer_specific_report_t {
    uint16_t manufacturer_id; /**< Manufacturer ID */
    uint16_t product_type;    /**< Product Type ID */
    uint16_t product_id;      /**< Product ID */
} znet_manufacturer_specific_report_t;

/**
 * @brief Function prototype for notify: get manufacturer specific
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MANUFACTURER_SPECIFIC_RESULT )( znet_error_t err,
                                                              znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              const znet_manufacturer_specific_report_t* value,
                                                              void* arg );

/**
 * @brief Function prototype for notify: request manufacturer specific
 *
 * @param data Current data
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MANUFACTURER_SPECIFIC_FILL )( znet_manufacturer_specific_report_t* data, void* arg );

/**
 * @brief Device specific: Device ID Type
 */
typedef uint8_t znet_cmd_device_specific_type_t;

#define ZNET_CMD_DEVICE_SPECIFIC_TYPE_OEM 0
#define ZNET_CMD_DEVICE_SPECIFIC_TYPE_SERIAL 1
#define ZNET_CMD_DEVICE_SPECIFIC_TYPE_PSEUDO_RAND 2

/**
 * @brief Device specific: Data format
 */
typedef uint8_t znet_cmd_device_specific_format_t;

#define ZNET_CMD_DEVICE_SPECIFIC_FORMAT_UTF8 0x00
#define ZNET_CMD_DEVICE_SPECIFIC_FORMAT_BIN 0x01

/**
 * @brief Device specific
 */
typedef struct znet_device_specific_report_t {
    uint8_t __ver;                                 /**< reserved */
    znet_cmd_device_specific_type_t type;          /**< Device ID Type */
    znet_cmd_device_specific_format_t data_format; /**< Device ID Data Format */
    uint8_t data_count;                            /**< Device ID Data Length */
    uint8_t data[];                                /**< Device ID Data */
} znet_device_specific_report_t;

/**
 * @brief Function prototype for notify: get device specific
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_DEVICE_SPECIFIC_RESULT )( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        const znet_device_specific_report_t* value,
                                                        void* arg );

/**
 * @brief Z-Wave Plus Info: Role Type
 */
typedef uint8_t znet_zwaveplus_info_role_type_t;

/**
 * @brief Z-Wave Plus Info: Node Type
 */
typedef uint8_t znet_zwaveplus_info_node_type_t;

#define ZNET_ZWAVEPLUS_INFO_NODE_TYPE_NODE 0x00
#define ZNET_ZWAVEPLUS_INFO_NODE_TYPE_FOR_IP_GATEWAY 0x02
#define ZNET_ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_PC 0x02
#define ZNET_ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_PS 0x04
#define ZNET_ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_RSS 0x06
/**
 * @brief Z-Wave Plus Info: Installer Icon Type
 */
typedef uint16_t znet_zwaveplus_info_installer_icon_t;

/**
 * @brief Z-Wave Plus Info: User Icon Type
 */
typedef uint16_t znet_zwaveplus_info_user_icon_t;

/**
 * @brief Z-Wave Plus Info
 */
typedef struct znet_zwaveplus_info_report_t {
    uint8_t zwave_plus_ver; /**< Z-Wave Plus Version */
    znet_zwaveplus_info_role_type_t role_type;
    znet_zwaveplus_info_node_type_t node_type;
    znet_zwaveplus_info_installer_icon_t installer_icon;
    znet_zwaveplus_info_user_icon_t user_icon;
} znet_zwaveplus_info_report_t;

/**
 * @brief Function prototype for notify: get z-wave plus info
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ZWAVEPLUS_INFO_RESULT )( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       const znet_zwaveplus_info_report_t* value,
                                                       void* arg );

/**
 * @brief Function prototype for notify: request z-wave plus info
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ZWAVEPLUS_INFO_FILL )( znet_zwaveplus_info_report_t* data, void* arg );

typedef struct znet_firmware_info_t {
    uint16_t manufacturer_id;
    uint16_t firmware_id;
    uint16_t checksum;
} znet_firmware_info_t;

typedef znet_firmware_info_t znet_firmware_md_report_v1_t;

typedef struct znet_firmware_md_report_v3_t {
    bool firmware_upgradable;
    uint16_t max_fragment_size;
    uint8_t additional_firmware_count;
    uint16_t* additional_firmware;
} znet_firmware_md_report_v3_t;

typedef struct znet_firmware_md_report_v5_t {
    uint8_t hardware_revision;
} znet_firmware_md_report_v5_t;

typedef void ( *ZNET_NODE_CMD_FIRMWARE_UPDATE_INFO_FILL )( znet_firmware_md_report_v1_t* dataV1,
                                                           znet_firmware_md_report_v3_t* dataV3,
                                                           znet_firmware_md_report_v5_t* dataV5,
                                                           void* arg );

/**
 * @brief TBD.
 */
typedef uint8_t znet_cmd_basic_value_t;

#define ZNET_CMD_BASIC_VALUE_MIN 0
#define ZNET_CMD_BASIC_VALUE_MAX 0x63
#define ZNET_CMD_BASIC_VALUE_UNKNOWN 0xFE
#define ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE 0xFF

/**
 * @brief Function prototype for notify: node cmd basic report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BASIC_RESULT )( znet_error_t err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              znet_cmd_basic_value_t currentValue,
                                              znet_cmd_basic_value_t targetValue,
                                              uint8_t duration,
                                              void* arg );

/**
 * @brief TBD.
 */
typedef uint8_t znet_cmd_battery_level_value_t;

#define ZNET_CMD_BATTERY_LEVEL_MIN 0
#define ZNET_CMD_BATTERY_LEVEL_MAX 0x64
#define ZNET_CMD_BATTERY_LEVEL_UNKNOWN 0xFE
#define ZNET_CMD_BATTERY_LEVEL_LOW_WARN 0xFF

/**
 * @brief Function prototype for notify: node cmd battery level report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BATTERY_LEVEL_RESULT )( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      znet_cmd_battery_level_value_t value,
                                                      void* arg );

/**
 * @brief TBD.
 */
typedef uint8_t znet_cmd_binary_switch_value_t;

#define ZNET_CMD_BINARY_SWITCH_VALUE_MIN 0
#define ZNET_CMD_BINARY_SWITCH_VALUE_MAX 0x63
#define ZNET_CMD_BINARY_SWITCH_VALUE_UNKNOWN 0xFE
#define ZNET_CMD_BINARY_SWITCH_VALUE_MAX_ABSOLUTE 0xFF

/**
 * @brief Function prototype for notify: node cmd bianry switch report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BINARY_SWITCH_RESULT )( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      znet_cmd_binary_switch_value_t value,
                                                      znet_cmd_binary_switch_value_t target_value,
                                                      uint8_t duration,
                                                      void* arg );

/**
 * @brief Sound Switch
 */
typedef uint8_t znet_cmd_sound_switch_tones_number_t;
typedef uint8_t znet_cmd_sound_switch_tone_id_t;

typedef struct znet_cmd_sound_switch_tone_info_t {
    znet_cmd_sound_switch_tone_id_t tone_id;
    uint16_t tone_duration;
    uint8_t name_length;
    const uint8_t* name;
} znet_cmd_sound_switch_tone_info_t;

typedef struct znet_cmd_sound_switch_configuration_t {
    uint8_t volume;
    znet_cmd_sound_switch_tone_id_t default_tone_id;
} znet_cmd_sound_switch_configuration_t;

typedef struct znet_cmd_sound_switch_tone_play_t {
    znet_cmd_sound_switch_tone_id_t tone_id;
    uint8_t volume;
} znet_cmd_sound_switch_tone_play_t;

#define ZNET_CMD_SOUND_SWITCH_TONE_ID_MIN 1
#define ZNET_CMD_SOUND_SWITCH_TONE_ID_MAX 0xFE

/**
 * @brief Tones Number Report
 */
typedef void ( *ZNET_NODE_CMD_SOUND_SWITCH_TONES_NUMBER_RESULT )( znet_error_t err,
                                                                  znet_node_id_t node_id,
                                                                  znet_node_channel_id_t channel_id,
                                                                  znet_cmd_sound_switch_tones_number_t value,
                                                                  void* arg );

/**
 * @brief Tone Info Report
 */
typedef void ( *ZNET_NODE_CMD_SOUND_SWITCH_TONE_INFO_RESULT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_cmd_sound_switch_tone_id_t tone_id,
                                                               const znet_cmd_sound_switch_tone_info_t* value,
                                                               void* arg );

/**
 * @brief Configuration Report
 */
typedef void ( *ZNET_NODE_CMD_SOUND_SWITCH_CONFIGURATION_RESULT )( znet_error_t err,
                                                                   znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   const znet_cmd_sound_switch_configuration_t* value,
                                                                   void* arg );

/**
 * @brief Tone Play Report
 */
typedef void ( *ZNET_NODE_CMD_SOUND_SWITCH_TONE_PLAY_RESULT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               const znet_cmd_sound_switch_tone_play_t* value,
                                                               void* arg );

/**
 *@brief TBD.
 */
typedef uint8_t znet_clock_hour_value_t;
typedef uint8_t znet_clock_minute_value_t;
typedef uint8_t znet_clock_weekday_value_t;

typedef struct znet_clock_value_t {
    znet_clock_hour_value_t hour;
    znet_clock_minute_value_t minute;
    znet_clock_weekday_value_t weekday;
} znet_clock_value_t;

#define ZNET_CMD_CLOCK_HOUR_VALUE_MIN 0
#define ZNET_CMD_CLOCK_HOUR_VALUE_MAX 23
#define ZNET_CMD_CLOCK_MINUTE_VALUE_MIN 0
#define ZNET_CMD_CLOCK_MINUTE_VALUE_MAX 59
#define ZNET_CMD_CLOCK_WEEKDAY_VALUE_UNUSED_UNKNOWN 0x00
#define ZNET_CMD_CLOCK_WEEKDAY_VALUE_MIN 0x01
#define ZNET_CMD_CLOCK_WEEKDAY_VALUE_MAX 0x07

/**
 *@brief Function prototype for notification: node clock report
 */
typedef void ( *ZNET_NODE_CMD_CLOCK_RESULT )( znet_error_t err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              const znet_clock_value_t* value,
                                              void* arg );

typedef uint16_t znet_cmd_time_parameters_year_t;
typedef uint8_t znet_cmd_time_parameters_month_t;
typedef uint8_t znet_cmd_time_parameters_day_t;
typedef uint8_t znet_cmd_time_parameters_hour_t;
typedef uint8_t znet_cmd_time_parameters_minute_t;
typedef uint8_t znet_cmd_time_parameters_second_t;

#define ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MIN 1   // January
#define ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MAX 12  // December
#define ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MIN 1
#define ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MAX 31
#define ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MIN 0
#define ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MAX 23
#define ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MIN 0
#define ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MAX 59
#define ZNET_CMD_TIME_PARAMETERS_SECOND_VALUE_MIN 0
#define ZNET_CMD_TIME_PARAMETERS_SECOND_VALUE_MAX 59

typedef struct znet_time_t {
    znet_cmd_time_parameters_hour_t hour;
    znet_cmd_time_parameters_minute_t minute;
    znet_cmd_time_parameters_second_t second;
} znet_time_t;

typedef struct znet_date_t {
    znet_cmd_time_parameters_year_t year;
    znet_cmd_time_parameters_month_t month;
    znet_cmd_time_parameters_day_t day;
} znet_date_t;

static inline bool is_time_valid( const znet_time_t* t ) {
    return /* t->hour >= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MIN && */
        t->hour <= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MAX &&
        /* t->minute >= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MIN && */
        t->minute <= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MAX &&
        /* t->second >= ZNET_CMD_TIME_PARAMETERS_SECOND_VALUE_MIN && */
        t->second <= ZNET_CMD_TIME_PARAMETERS_SECOND_VALUE_MAX;
}

static inline bool is_date_valid( const znet_date_t* d ) {
    return d->month >= ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MIN &&
           d->month <= ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MAX && d->day >= ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MIN &&
           d->day <= ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MAX;
}

typedef struct znet_date_time_t {
    znet_date_t date;
    znet_time_t time;
} znet_date_time_t;

typedef struct znet_cmd_time_parameters_report_t {
    znet_date_t date_utc;
    znet_time_t time_utc;
} znet_cmd_time_parameters_report_t;

/**
 *@brief Function prototype for notification: node time parameters report
 */
typedef void ( *ZNET_NODE_CMD_TIME_PARAMETERS_RESULT )( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        const znet_cmd_time_parameters_report_t* value,
                                                        void* arg );

/**
 * @brief Doorlock configuration report
 */
typedef struct znet_door_lock_extended_configuration_t {
    uint16_t auto_relock_timeout;
    uint16_t hold_and_release_timeout;
    bool twist_assist_enabled;
    bool block_to_block_enabled;
} znet_door_lock_extended_configuration_t;

typedef struct znet_door_lock_configuration_t {
    uint8_t enabled_inside_handle_ids;  /**< Status of each individual inside door handle  */
    uint8_t enabled_outside_handle_ids; /**< Status of each individual outside door handle  */
    uint16_t lock_timeout; /**< Remaining time before the door lock will automatically be locked again in seconds */
    bool are_timed_operations_enabled;
} znet_door_lock_configuration_t;

/**
 * @brief Doorlock capabilities report
 */
typedef struct znet_door_lock_capabilities_t {
    uint8_t supported_inside_handle_ids;
    uint8_t supported_outside_handle_ids;
    bool is_door_supported : 1;
    bool is_bolt_supported : 1;
    bool is_latch_supported : 1;
    bool is_block_to_block_supported : 1;
    bool is_twist_assist_supported : 1;
    bool is_hold_and_release_supported : 1;
    bool is_auto_relock_supported : 1;
    bool are_timed_operations_supported : 1;
    uint8_t supported_door_lock_modes_count;
    const uint8_t* supported_door_lock_modes;
} znet_door_lock_capabilities_t;

/**
 * @brief Function prototype for notify: node cmd door lock configuration
 */
typedef void ( *ZNET_NODE_CMD_DOOR_LOCK_CONFIGURATION_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_door_lock_configuration_t* configuration,
    const znet_door_lock_extended_configuration_t* extended_configuration,
    void* arg );

/**
 * @brief Function prototype for notify: node cmd door lock capabilities
 */
typedef void ( *ZNET_NODE_CMD_DOOR_LOCK_CAPABILITIES_RESULT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               const znet_door_lock_capabilities_t* value,
                                                               void* arg );

/**
 * @brief Values used for Door Lock Operation Report command.
 */

#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED 0x00
#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_WITH_TIMEOUT 0x01
#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES 0x10
#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT 0x11
#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES 0x20
#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT 0x21
#define ZNET_CMD_DOOR_LOCK_OPERATION_REPORT_DOOR_SECURED 0xFF

/**
 * @brief Doorlock operation report
 */
typedef struct znet_door_lock_operation_report_t {
    uint8_t mode;                       /**< Mode of the door lock device  */
    uint8_t enabled_inside_handle_ids;  /**< Status of each individual inside door handle  */
    uint8_t enabled_outside_handle_ids; /**< Status of each individual outside door handle  */
    uint8_t condition;                  /**< Status of the door lock components  */
    bool is_lock_timeout_activated;
    uint16_t lock_timeout; /**< Remaining time before the door lock will automatically be locked again ( sec ) */
    bool has_target_mode;
    uint8_t tagret_mode;         /**< Target mode of an ongoing transition */
    uint8_t tagret_mode_timeout; /**< Remaining time before the target mode is reached */
} znet_door_lock_operation_report_t;

/**
 * @brief Function prototype for notify: node cmd door lock operation
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_DOOR_LOCK_OPERATION_RESULT )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            const znet_door_lock_operation_report_t* value,
                                                            void* arg );

typedef uint8_t znet_cmd_door_lock_operation_set_t;

/**
 * @brief Supported Users
 */
typedef uint16_t znet_supported_users_t;

/**
 * @brief User Code Report
 */
typedef struct znet_user_code_report_t {
    znet_supported_users_t user_id; /**< User Identifier */
    uint8_t user_id_status;         /**< Status of the User Identifier */
    uint8_t user_code_size;         /**< Length of the User Codes field in bytes */
    const uint8_t* user_code;
} znet_user_code_report_t;

/**
 * @brief Function prototype for notify: node cmd user code report
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_USER_CODE_REPORT )( znet_error_t err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t requested_user_id,
                                                  const znet_user_code_report_t* value,
                                                  void* arg );

/**
 * @brief Function prototype for notify: node cmd user code report
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_USERS_NUMBER_REPORT )( znet_error_t err,
                                                     znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     znet_supported_users_t supported_users,
                                                     void* arg );

typedef struct znet_user_id_status_capability_report_t {
    uint8_t MCD_support : 1;
    uint8_t MC_support : 1;
    uint8_t bitmask_length : 5;
    const uint8_t* bitmask;
} znet_user_id_status_capability_report_t;

typedef struct znet_keypad_modes_capability_report_t {
    uint8_t MUCS_support : 1;
    uint8_t MUCR_support : 1;
    uint8_t UCC_support : 1;
    uint8_t bitmask_length : 5;
    const uint8_t* bitmask;
} znet_keypad_modes_capability_report_t;

typedef struct znet_keys_capability_report_t {
    uint8_t bitmask_length : 5;
    const uint8_t* bitmask;
} znet_keys_capability_report_t;

typedef struct znet_user_code_capabilities_report_t {
    znet_user_id_status_capability_report_t user_id_status_capability;
    znet_keypad_modes_capability_report_t keypad_modes_capability;
    znet_keys_capability_report_t keys_capability;
} znet_user_code_capabilities_report_t;

typedef void ( *ZNET_NODE_CMD_USER_CODE_CAPABILITIES_REPORT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_user_code_capabilities_report_t* user_code_capabilities,
    void* arg );

typedef uint8_t znet_user_code_keypad_mode_t;

typedef void ( *ZNET_NODE_CMD_USER_CODES_KEYPAD_MODE_REPORT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_user_code_keypad_mode_t keypad_mode,
                                                               void* arg );

typedef void ( *ZNET_NODE_CMD_MASTER_CODE_REPORT )( znet_error_t err,
                                                    znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    uint8_t code_length,
                                                    const uint8_t* code,
                                                    void* arg );

typedef void ( *ZNET_NODE_CMD_EXTENDED_USER_CODE_REPORT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           znet_supported_users_t requested_user_id,
                                                           uint8_t user_codes_count,
                                                           const znet_user_code_report_t* user_codes,
                                                           znet_supported_users_t next_user_code,
                                                           void* arg );

typedef uint16_t znet_user_code_checksum_t;
typedef void ( *ZNET_NODE_CMD_USER_CODE_CHECKSUM_REPORT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           znet_user_code_checksum_t checksum,
                                                           void* arg );

/**
 * @brief Entry Control Notification
 */

typedef struct znet_entry_control_notification_t {
    uint8_t __ver;             /**< reserved */
    uint8_t sequence_number;   /**< Sequence number */
    uint8_t data_type;         /**< Type of data (if any) that is appended to this command */
    uint8_t event_type;        /**< Advertise the actual event type */
    uint8_t event_data_length; /**< Length of the Event Data field in bytes */
    uint8_t event_data[];
} znet_entry_control_notification_t;

/**
 * @brief Function prototype for notify: node cmd entry control notification
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ENTRY_CONTROL_NOTIFICATION )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            znet_entry_control_notification_t* value,
                                                            void* arg );

/**
 * @brief Entry Control Key Supported Report
 */

typedef struct znet_entry_control_key_supported_report_t {
    uint8_t __ver;                         /**< reserved */
    uint8_t key_supported_bit_mask_length; /**< Length of Bit Mask  */
    uint8_t key_supported_bit_mask[];
} znet_entry_control_key_supported_report_t;

/**
 * @brief Function prototype for notify: node cmd entry control key supported report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ENTRY_CONTROL_KEY_SUPPORTED_REPORT )( znet_error_t err,
                                                                    znet_node_id_t node_id,
                                                                    znet_node_channel_id_t channel_id,
                                                                    znet_entry_control_key_supported_report_t* value,
                                                                    void* arg );

/**
 * @brief Entry Control Event Supported Report
 */

typedef struct znet_data_type_supported_t {
    uint8_t bit_mask_length; /* Number of Data Type Supported Bit Mask Bytes */
    uint8_t bit_mask[];
} znet_data_type_supported_t;

typedef struct znet_event_type_supported_t {
    uint8_t bit_mask_length; /* Number of Event Type Supported Bit Mask Bytes */
    uint8_t bit_mask[];
} znet_event_type_supported_t;

typedef struct znet_entry_control_event_supported_report_t {
    uint8_t __ver; /**< reserved */
    znet_data_type_supported_t* data_type_supported;
    znet_event_type_supported_t* event_type_supported;
    struct key_cached {
        uint8_t size_supported_min;    /* The minimum configurable number of key entries */
        uint8_t size_supported_max;    /* The maximum configurable number of key entries */
        uint8_t timeout_supported_min; /* The minimum configurable timeout */
        uint8_t timeout_supported_max; /* The maximum configurable timeout */
    } key_cached;
} znet_entry_control_event_supported_report_t;

/**
 * @brief Function prototype for notify: node cmd entry control event supported report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ENTRY_CONTROL_EVENT_SUPPORTED_REPORT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    znet_entry_control_event_supported_report_t* value,
    void* arg );

/**
 * @brief Entry Control Configuration Report
 */

typedef struct znet_entry_control_configuration_report_t {
    uint8_t __ver;             /**< reserved */
    uint8_t key_cache_size;    /**< Number of characters the key cache MUST store before sending data */
    uint8_t key_cache_timeout; /**< Number of seconds the key cache MUST wait for additional characters before
                                   sending data */
} znet_entry_control_configuration_report_t;

/**
 * @brief Function prototype for notify: node cmd entry control key supported report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ENTRY_CONTROL_CONFIGURATION_REPORT )( znet_error_t err,
                                                                    znet_node_id_t node_id,
                                                                    znet_node_channel_id_t channel_id,
                                                                    znet_entry_control_configuration_report_t* value,
                                                                    void* arg );

/**
 * @brief Entry Control Configuration Set
 */

typedef znet_entry_control_configuration_report_t znet_entry_control_configuration_set_t;

/**
 * @brief Notification Report - V8 (4 byte)
 */

typedef struct znet_notification_report_t {
    uint8_t v1_alarm_type;           /**< Alarm type */
    uint8_t v1_alarm_level;          /**< Alarm level  */
    uint8_t notification_status;     /**< Notification Status  */
    uint8_t notification_type;       /**< Notification Type  */
    uint8_t notification_event;      /**< Notification Event / State */
    uint8_t sequence;                /**< Advertise the presence of the "Sequence Number" field */
    uint8_t event_parameters_length; /**< Parameters length */
    uint8_t sequence_number;         /**< Sequence Number */
    const uint8_t* event_parameter;
} znet_notification_report_t;

/**
 * @brief Function prototype for notify: node cmd notification report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NOTIFICATION_REPORT )( znet_error_t err,
                                                     znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     uint8_t v1_alarm_type,
                                                     uint8_t notification_type,
                                                     uint8_t event_type,
                                                     const znet_notification_report_t* value,
                                                     void* arg );
/**
 * @brief Notification supported report. Versions Alarm V2, Notifications V3 ... V8
 *
 */
typedef struct znet_notification_supported_report_t {
    uint8_t v1_alarm;         // Indicates if node supports [1]: V1 and V2 Alarm notifications or [0]: V2 and newer
    uint8_t bit_mask_count;   // Number of bit masks
    const uint8_t* bit_mask;  // Supported Notification Types
} znet_notification_supported_report_t;

/**
 * @brief Function prototype for notify: node cmd notification supported report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NOTIFICATION_SUPPORTED_REPORT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               const znet_notification_supported_report_t* value,
                                                               void* arg );

/**
 * @brief Function prototype for notify: node cmd notification event supported report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NOTIFICATION_EVENT_SUPPORTED_REPORT )( znet_error_t err,
                                                                     znet_node_id_t node_id,
                                                                     znet_node_channel_id_t channel_id,
                                                                     uint8_t type,
                                                                     const uint8_t* bitmask,
                                                                     uint8_t size,
                                                                     void* arg );

/**
 * @brief Function prototype for notify: node cmd hail report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_HAIL_REPORT )( znet_error_t err, znet_node_id_t node_id, void* arg );

/**
 * @brief TBD.
 */
typedef uint8_t znet_cmd_multilevel_switch_value_t;

#define ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MIN 0
#define ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX 0x63
#define ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN 0xFE
#define ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE 0xFF
#define ZNET_CMD_MULTILEVEL_SWITCH_DEFAULT_DURATION 0xFF
#define ZNET_CMD_MULTILEVEL_SWITCH_VALUE_DELTA 1
#define ZNET_CMD_MULTILEVEL_SWITCH_REQ_LIMIT 3
#define ZNET_CMD_MULTILEVEL_SWITCH_ON_OFF 1
#define ZNET_CMD_MULTILEVEL_SWITCH_UP_DOWN 2
#define ZNET_CMD_MULTILEVEL_SWITCH_STOP 2
#define ZNET_CMD_MULTILEVEL_SWITCH_DIRECTION 1
/**
 * @brief Function prototype for notify: node cmd multilevel switch
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTILEVEL_SWITCH_RESULT )( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          znet_cmd_multilevel_switch_value_t currentValue,
                                                          znet_cmd_multilevel_switch_value_t targetValue,
                                                          uint8_t duration,
                                                          void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTILEVEL_SWITCH_SET_DONE )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            void* arg );

/**
 * @brief Multilevel switch supported report
 *
 */
typedef struct znet_multilevel_switch_supported_report_t {
    uint8_t primary_type;    // Primary Switch type
    uint8_t secondary_type;  // Secondary Switch type
} znet_multilevel_switch_supported_report_t;

/**
 * @brief Function prototype for notify: node cmd multilevel switch supported report
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTILEVEL_SWITCH_SUPPORTED_REPORT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_multilevel_switch_supported_report_t* value,
    void* arg );

/**
 * @brief Function prototype for multilevel sensor report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param data pointer to array with bit mask supported sensor types.
 * @param len length of array.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_MULTILEVEL_SENSOR_GET_SUPPORTED_SENSOR_RESULT )( znet_error_t err,
                                                                               znet_node_id_t node_id,
                                                                               znet_node_channel_id_t channel_id,
                                                                               const uint8_t* data,
                                                                               uint8_t len,
                                                                               void* arg );

/**
 * @brief Function prototype for multilevel sensor supported scale report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param sensor_type indicate the actual Sensor Type.
 * @param scale_mask advertise the supported scales for the actual sensor type.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_MULTILEVEL_SENSOR_GET_SUPPORTED_SCALE_RESULT )( znet_error_t err,
                                                                              znet_node_id_t node_id,
                                                                              znet_node_channel_id_t channel_id,
                                                                              uint8_t sensor_type,
                                                                              uint8_t scale_mask,
                                                                              void* arg );

/**
 * @brief Multilevel Sensor Report structure
 */
typedef struct znet_multilevel_sensor_report_t {
    uint8_t type;      /**< Sensor type */
    uint8_t precision; /**< Indicate how many decimal places are included the Sensor Value field */
    uint8_t scale;     /**< Indicate what scale is used for the actual sensor reading */
    uint8_t size;      /**< Length in bytes of the Sensor Value field */
    int32_t value;     /**< Value of the actual sensor reading */
} znet_multilevel_sensor_report_t;

/**
 * @brief Function prototype for multilevel sensor report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param value report structure.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_MULTILEVEL_SENSOR_GET_RESULT )( znet_error_t err,
                                                              znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              uint8_t type,
                                                              uint8_t scale,
                                                              const znet_multilevel_sensor_report_t* value,
                                                              void* arg );

/**
 * @brief Barrier Operator
 */

typedef uint8_t znet_cmd_barrier_operator_value_t;
typedef uint8_t znet_cmd_barrier_operator_signal_t;
typedef uint8_t znet_cmd_barrier_operator_signal_state_t;

#define ZNET_CMD_BARRIER_OPERATOR_VALUE_STOP_POSITION_L 0x01
#define ZNET_CMD_BARRIER_OPERATOR_VALUE_STOP_POSITION_H 0x63

#define ZNET_CMD_BARRIER_OPERATOR_SIGNAL_SIREN 0x01
#define ZNET_CMD_BARRIER_OPERATOR_SIGNAL_LIGHT 0x02

/**
 * @brief Function prototype for notify: node cmd barrier operator
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BARRIER_OPERATOR_RESULT )( znet_error_t err,
                                                         znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         znet_cmd_barrier_operator_value_t value,
                                                         void* arg );

/**
 * @brief Function prototype for notify: node cmd barrier operator signal
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param signalId Subsystem type
 * @param state Subsystem state
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BARRIER_OPERATOR_SIGNAL_RESULT )( znet_error_t err,
                                                                znet_node_id_t node_id,
                                                                znet_node_channel_id_t channel_id,
                                                                znet_cmd_barrier_operator_signal_t signalId,
                                                                znet_cmd_barrier_operator_signal_state_t state,
                                                                void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BARRIER_OPERATOR_SET_DONE )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           void* arg );

/**
 * @brief Function prototype for notify: node cmd barrier operator supported signals
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param data Bitmask
 * @param len Bitmask length
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_BARRIER_OPERATOR_SUPPORTED_SIGNALS_RESULT )( znet_error_t err,
                                                                           znet_node_id_t node_id,
                                                                           znet_node_channel_id_t channel_id,
                                                                           const uint8_t* data,
                                                                           uint8_t len,
                                                                           void* arg );

/**
 * @brief Meter type specifies what type of metering device.
 */
#define ZNET_METER_TYPE_ELECTRIC 0x01
#define ZNET_METER_TYPE_GAS 0x02
#define ZNET_METER_TYPE_WATER 0x03

/**
 * @brief Scale advertise the unit used.
 */
#define ZNET_METER_SCALE_ELECTRIC_kWh 0
#define ZNET_METER_SCALE_ELECTRIC_kVAh 1
#define ZNET_METER_SCALE_ELECTRIC_W 2
#define ZNET_METER_SCALE_ELECTRIC_PULSE_COUNT 3
#define ZNET_METER_SCALE_ELECTRIC_V 4
#define ZNET_METER_SCALE_ELECTRIC_A 5
#define ZNET_METER_SCALE_ELECTRIC_POWER_FACTOR 6
#define ZNET_METER_SCALE_ELECTRIC_EX 7
#define ZNET_METER_SCALE_ELECTRIC_kVar 7
#define ZNET_METER_SCALE_ELECTRIC_kVarh 8
#define ZNET_METER_SCALE_GAS_CUBIC_METERS 0
#define ZNET_METER_SCALE_GAS_CUBIC_FEET 1
#define ZNET_METER_SCALE_GAS_PULSE_COUNT 3
#define ZNET_METER_SCALE_WATER_CUBIC_METERS 0
#define ZNET_METER_SCALE_WATER_CUBIC_FEET 1
#define ZNET_METER_SCALE_WATER_US_GALLONS 2
#define ZNET_METER_SCALE_WATER_PULSE_COUNT 3
#define ZNET_METER_SCALE_HEATING_kWh 0
#define ZNET_METER_SCALE_COOLING_kWh 0

/**
 * @brief Rate type specifies if it is import or export values to be read.
 *
 * Setting the rate type to import on the meter report is an indication
 * that the meter value is a consumed measurement. In contrary when
 * the rate type is set to export the indication of the meter value is
 * a produced measurement.
 */
#define ZNET_METER_RATE_TYPE_IMPORT 0x01
#define ZNET_METER_RATE_TYPE_EXPORT 0x02

/**
 * @brief Meter report
 */
typedef struct znet_meter_report_t {
    uint8_t type;       /**< Meter type */
    uint8_t scale;      /**< Scale of values */
    int32_t value;      /**< Current value */
    uint8_t precision;  /**< Precision (real value = value / (10 ^ precision)) */
    uint8_t rate_type;  /**< Rate type */
    uint16_t dtime;     /**< Delta time between previos and current value */
    int32_t prev_value; /**< Previos value */
} znet_meter_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_METER_RESULT )( znet_error_t err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              uint8_t scale,
                                              const znet_meter_report_t* value,
                                              void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_METER_SET_DONE )( znet_error_t err,
                                                znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                void* arg );

/**
 * @brief Meter supported report
 */
typedef struct znet_meter_supported_report_t {
    uint8_t type;                  /**< Meter type */
    uint8_t reset;                 /**< flag: support meter erset */
    uint8_t rate_import;           /**< flag: support rate type import */
    uint8_t rate_export;           /**< flag: support rate type export */
    uint8_t scales_bitmask_size;   /**< Scales count */
    const uint8_t* scales_bitmask; /**< flag: support scale */
} znet_meter_supported_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_METER_SUPPORTED_RESULT )( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        const znet_meter_supported_report_t* value,
                                                        void* arg );

/**
 * @brief Multi channel end point report
 */
typedef struct znet_multichannel_endpoint_report_t {
    uint8_t dynamic;                     /**< flag: implemented a dynamic number of end points */
    uint8_t identical;                   /**< flag: all end points have identical capabilities */
    uint8_t channel_ids_count;           /**< End points count */
    uint8_t agregated_channel_ids_count; /**< Agregated end points count */
} znet_multichannel_endpoint_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_ENDPOINT_RESULT )( znet_error_t err,
                                                              znet_node_id_t node_id,
                                                              const znet_multichannel_endpoint_report_t* value,
                                                              void* arg );

/**
 * @brief Multi channel capability report
 */
typedef struct znet_multichannel_capability_report_t {
    uint8_t dynamic;                      /**< flag: Channel ID is dynamic */
    znet_node_channel_id_t channel_id;    /**< Channel ID */
    znet_generic_class_t generic;         /**< Generic class type */
    znet_specific_class_t specific;       /**< Specific class type */
    uint8_t commands_count;               /**< Commands classes count */
    const znet_command_class_t* commands; /**< Commands classes, max 35 */
} znet_multichannel_capability_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_CAPABILITY_RESULT )( znet_error_t err,
                                                                znet_node_id_t node_id,
                                                                znet_node_channel_id_t channel_id,
                                                                const znet_multichannel_capability_report_t* value,
                                                                void* arg );

/**
 * @brief Multi channel end point report
 */
typedef struct znet_multichannel_endpoint_find_report_t {
    uint8_t reports_to_follow;            /**< Reports to follow */
    znet_generic_class_t generic;         /**< Generic class type */
    znet_specific_class_t specific;       /**< Specific class type */
    uint8_t channel_ids_count;            /**< End points count */
    znet_node_channel_id_t channel_ids[]; /**< End points */
} znet_multichannel_endpoint_find_report_t;

/**
 * @brief TBD.
 *
 * It is called many times. value is NULL if
 * there are no more reports.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_ENDPOINT_FIND_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    const znet_multichannel_endpoint_find_report_t* value,
    void* arg );

/**
 * @brief Multi Channel Aggregated Members report
 */
typedef struct znet_multichannel_aggregated_members_report_t {
    uint8_t __ver;                                  /**< reserved */
    znet_node_channel_id_t channel_id;              /**< Channel ID */
    uint8_t agregated_channel_ids_count;            /**< End points count */
    znet_node_channel_id_t agregated_channel_ids[]; /**< End points */
} znet_multichannel_aggregated_members_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_AGGREGATED_MEMBERS_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_multichannel_aggregated_members_report_t* value,
    void* arg );

/**
 * @brief Configuration specific: Parameter types
 */

typedef uint16_t znet_cmd_configuration_id_t;
typedef uint32_t znet_cmd_configuration_value_t;

#define ZNET_CMD_CONFIGURATION_PARAM_NUM_INVALID 0x03
#define ZNET_CMD_CONFIGURATION_PARAM_NUM_MAX 0x04
#define ZNET_CMD_CONFIGURATION_REPORT_CHECK_LEN 0x04

/**
 * @brief Configuration reports: min length constants
 * ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN - parameter
 * for Name Report and Info Report;
 */
#define ZNET_CMD_CONFIGURATION_BULK_REPORT_CHECK_LEN 0x07
#define ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN 0x05
#define ZNET_CMD_CONFIGURATION_PROP_REPORT_CHECK_LEN 0x06
#define ZNET_CMD_CONFIGURATION_PROP_REPORT_COMMON_PART 0x04

/**
 * @brief Configuration specific: Parameter types
 */
typedef struct znet_configuration_report_t {
    uint8_t param_number;                 /**< Parameter Number */
    uint8_t data_count;                   /**< Size of the actual parameter. */
    znet_cmd_configuration_value_t value; /**< Configuration Data */
} znet_configuration_report_t;

/**
 * @brief Configuration specific: Parameter types
 */
typedef struct znet_configuration_bulk_report_t {
    znet_cmd_configuration_id_t param_offset; /**< Parameter Offset */
    uint8_t params_count;                     /**< Parameter Number */
    uint8_t param_size;                       /**< Parameter Size */
    uint8_t rep_to_follows;                   /**< Report to follows */
    uint32_t values[];                        /**< Configuration Data */
} znet_configuration_bulk_report_t;

/**
 * @brief  Command Class, versions 3-4: Configuration Name Report
 */
typedef struct znet_configuration_name_report_t {
    znet_cmd_configuration_id_t param_number; /**< Parameter Number */
    uint8_t rep_to_follows;                   /**< Report to follows */
    uint8_t name_length;                      /**< Length */
    uint8_t data[];                           /**< Configuration Data */
} znet_configuration_name_report_t;

/**
 * @brief  Command Class, versions 3-4: Configuration Info Report
 */
typedef struct znet_configuration_info_report_t {
    znet_cmd_configuration_id_t param_number; /**< Parameter Number */
    uint8_t rep_to_follows;                   /**< Report to follows */
    uint16_t info_length;                     /**< Length */
    uint8_t data[];                           /**< Configuration Data */
} znet_configuration_info_report_t;

/**
 * @brief  Command Class, versions 3-4: Configuration Properties Report
 */
typedef struct znet_configuration_properties_report_t {
    znet_cmd_configuration_id_t param_number;      /**< Parameter Number */
    znet_cmd_configuration_id_t next_param_number; /**< Parameter Number */
    uint8_t param_format;                          /**< Format of the actual parameter. */
    uint8_t param_size;                            /**< Size of the actual parameter. */
    uint32_t param_min_value;                      /**< Min value of the actual parameter. */
    uint32_t param_max_value;                      /**< Max value of the actual parameter. */
    uint32_t param_default_value;                  /**< Default value of the actual parameter. */
    bool read_only;                                /**< Is parameter read only*/
} znet_configuration_properties_report_t;

/**
 * @brief Function prototype for notify: node cmd configuration report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current configuration value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CONFIGURATION_RESULT )( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint8_t parameter_number,
                                                      const znet_configuration_report_t* value,
                                                      void* arg );

typedef void ( *ZNET_NODE_CMD_CONFIGURATION_REQUEST_STATUS )( znet_error_t err,
                                                              znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              uint8_t parameter_number,
                                                              void* arg );

/**
 * @brief Function prototype for notify: node cmd configuration bulk
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current configuration value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CONFIGURATION_BULK_RESULT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           znet_cmd_configuration_id_t param_number,
                                                           uint8_t config_count,
                                                           const znet_configuration_bulk_report_t* value,
                                                           void* arg );

/**
 * @brief Function prototype for notify: node cmd configuration name
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current configuration value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CONFIGURATION_NAME_RESULT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           znet_cmd_configuration_id_t param_number,
                                                           const znet_configuration_name_report_t* value,
                                                           void* arg );

/**
 * @brief Function prototype for notify: node cmd configuration info
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current configuration value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CONFIGURATION_INFO_RESULT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           znet_cmd_configuration_id_t param_number,
                                                           const znet_configuration_info_report_t* value,
                                                           void* arg );

/**
 * @brief Function prototype for notify: node cmd configuration info
 * report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param config_value Current configuration value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CONFIGURATION_PROPERTIES_RESULT )( znet_error_t err,
                                                                 znet_node_id_t node_id,
                                                                 znet_node_channel_id_t channel_id,
                                                                 znet_cmd_configuration_id_t param_number,
                                                                 const znet_configuration_properties_report_t* value,
                                                                 void* arg );

/**
 * @brief Sensor Alarm
 */

#define ZNET_CMD_ALARM_SENSOR_GENERALPURPOSE 0x00
#define ZNET_CMD_ALARM_SENSOR_SMOKE 0x01
#define ZNET_CMD_ALARM_SENSOR_CO 0x02
#define ZNET_CMD_ALARM_SENSOR_CO2 0x03
#define ZNET_CMD_ALARM_SENSOR_HEAT 0x04
#define ZNET_CMD_ALARM_SENSOR_WATERLEAK 0x05
#define ZNET_CMD_ALARM_SENSOR_1STSUPPORTEDSENSOR 0xFF

/**
 * @brief Function prototype for alarm sensor report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param channel_id Channel ID
 * @param value sensor value.
 * @param sensor_type specifies what type of sensor.
 * @param seconds time the remote alarm.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_ALARM_SENSOR_GET_RESULT )( znet_error_t err,
                                                         znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         uint8_t sensor_type,
                                                         uint8_t value,
                                                         uint16_t seconds,
                                                         void* arg );

/**
 * @brief Function prototype for alarm sensor supported sensor report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param channel_id Channel ID
 * @param data pointer to array with bit mask supported sensor types.
 * @param len length of array.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_ALARM_SENSOR_GET_SUPPORTED_SENSOR_RESULT )( znet_error_t err,
                                                                          znet_node_id_t node_id,
                                                                          znet_node_channel_id_t channel_id,
                                                                          const uint8_t* data,
                                                                          uint8_t len,
                                                                          void* arg );

/**
 * @brief Binary Sensor
 */

#define ZNET_CMD_BINARY_SENSOR_RESERVED 0x00
#define ZNET_CMD_BINARY_SENSOR_GENERALPURPOSE 0x01
#define ZNET_CMD_BINARY_SENSOR_SMOKE 0x02
#define ZNET_CMD_BINARY_SENSOR_CO 0x03
#define ZNET_CMD_BINARY_SENSOR_CO2 0x04
#define ZNET_CMD_BINARY_SENSOR_HEAT 0x05
#define ZNET_CMD_BINARY_SENSOR_WATER 0x06
#define ZNET_CMD_BINARY_SENSOR_FREEZE 0x07
#define ZNET_CMD_BINARY_SENSOR_TAMPER 0x08
#define ZNET_CMD_BINARY_SENSOR_AUX 0x09
#define ZNET_CMD_BINARY_SENSOR_DOORWINDOW 0x0A
#define ZNET_CMD_BINARY_SENSOR_TILT 0x0B
#define ZNET_CMD_BINARY_SENSOR_MOTION 0x0C
#define ZNET_CMD_BINARY_SENSOR_GLASSBREAK 0x0D
#define ZNET_CMD_BINARY_SENSOR_1STSUPPORTEDSENSOR 0xFF

/**
 * @brief Function prototype for binary sensor report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param channel_id Channel ID
 * @param value sensor value.
 * @param sensor_type specifies what type of sensor.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_BINARY_SENSOR_GET_RESULT )( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          uint8_t sensor_type,
                                                          uint8_t value,
                                                          void* arg );

/**
 * @brief Function prototype for binary sensor supported sensor report command
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID.
 * @param channel_id Channel ID
 * @param data pointer to array with bit mask supported sensor types.
 * @param len length of array.
 * @param arg Parameter for callback functions.
 */
typedef void ( *ZNET_NODE_CMD_BINARY_SENSOR_GET_SUPPORTED_SENSOR_RESULT )( znet_error_t err,
                                                                           znet_node_id_t node_id,
                                                                           znet_node_channel_id_t channel_id,
                                                                           const uint8_t* data,
                                                                           uint8_t len,
                                                                           void* arg );

/**
 * @brief Color Switch value
 */
typedef uint8_t znet_color_switch_value_t;

#define ZNET_CMD_COLOR_SWITCH_VALUE_MIN 0x00
#define ZNET_CMD_COLOR_SWITCH_VALUE_MAX 0xFF

#define ZNET_CMD_COLOR_SWITCH_COLOR_WARM_WHITE 0x00
#define ZNET_CMD_COLOR_SWITCH_COLOR_COLD_WHITE 0x01
#define ZNET_CMD_COLOR_SWITCH_COLOR_RED 0x02
#define ZNET_CMD_COLOR_SWITCH_COLOR_GREEN 0x03
#define ZNET_CMD_COLOR_SWITCH_COLOR_BLUE 0x04
#define ZNET_CMD_COLOR_SWITCH_COLOR_AMBER 0x05
#define ZNET_CMD_COLOR_SWITCH_COLOR_CYAN 0x06
#define ZNET_CMD_COLOR_SWITCH_COLOR_PURPLE 0x07
#define ZNET_CMD_COLOR_SWITCH_COLOR_INDEXED 0x08

typedef struct znet_cl_component_t {
    uint8_t component_id;
    uint8_t value;
} znet_cl_component_t;

typedef struct znet_color_switch_report_t {
    uint8_t component_id;
    uint8_t value;
    bool has_target_value;
    uint8_t target_value;
    uint8_t target_duration;
} znet_color_switch_report_t;

/**
 * @brief Function prototype for notify: node cmd color switch report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param report Current color value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_COLOR_SWITCH_RESULT )( znet_error_t err,
                                                     znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     uint8_t color_component,
                                                     const znet_color_switch_report_t* report,
                                                     void* arg );

/**
 * @brief Report with supported color components
 */
typedef struct znet_color_switch_supported_report_t {
    uint8_t component_mask_count;  /**< number of masks with supported colors */
    const uint8_t* component_mask; /**< bitmask for supported color components */
} znet_color_switch_supported_report_t;

/**
 * @brief Function prototype for notify: node cmd color switch supported report/result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param report Mask of supported color components
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_COLOR_SWITCH_SUPPORTED_REPORT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               const znet_color_switch_supported_report_t* report,
                                                               void* arg );

/**
 * @brief Powerlevel value
 */
typedef uint8_t znet_powerlevel_value_t;

#define ZNET_POWERLEVEL_NORMAL 0x00
#define ZNET_POWERLEVEL_MINUS_1dBm 0x01
#define ZNET_POWERLEVEL_MINUS_2dBm 0x02
#define ZNET_POWERLEVEL_MINUS_3dBm 0x03
#define ZNET_POWERLEVEL_MINUS_4dBm 0x04
#define ZNET_POWERLEVEL_MINUS_5dBm 0x05
#define ZNET_POWERLEVEL_MINUS_6dBm 0x06
#define ZNET_POWERLEVEL_MINUS_7dBm 0x07
#define ZNET_POWERLEVEL_MINUS_8dBm 0x08
#define ZNET_POWERLEVEL_MINUS_9dBm 0x09

/**
 * @brief Powerlevel timeout
 */
typedef uint8_t znet_powerlevel_timeout_t;

#define ZNET_POWERLEVEL_TIMEOUT_MIN 1
#define ZNET_POWERLEVEL_TIMEOUT_MAX 255

/**
 * @brief Powerlevel report
 */
typedef struct znet_powerlevel_report_t {
    znet_powerlevel_value_t power_level; /**< Current power level */
    znet_powerlevel_timeout_t timeout;   /**< Timeout before back to normal power */

} znet_powerlevel_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_POWERLEVEL_RESULT )( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   const znet_powerlevel_report_t* value,
                                                   void* arg );

/**
 * @brief
 */
typedef uint8_t znet_powerlevel_test_status_t;

#define ZNET_POWERLEVEL_TEST_FAILED 0x00
#define ZNET_POWERLEVEL_TEST_SUCCESS 0x01
#define ZNET_POWERLEVEL_TEST_IN_PROGRESS 0x02

/**
 * @brief Powerlevel test node report
 */
typedef struct znet_powerlevel_test_node_report_t {
    znet_node_id_t test_id;               /**< Tested node ID */
    znet_powerlevel_test_status_t status; /**< Status of operation */
    uint16_t test_count_ack;              /**< Test frame acknowledged count */
} znet_powerlevel_test_node_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_POWERLEVEL_TEST_NODE_RESULT )( znet_error_t err,
                                                             znet_node_id_t node_id,
                                                             const znet_powerlevel_test_node_report_t* value,
                                                             void* arg );

#define ZNET_CMD_SCENE_SUPPORTED_REPORT_CHECK_LEN 0x04
typedef uint8_t znet_cmd_scene_id_t;
typedef uint8_t znet_cmd_scene_level_t;
typedef uint8_t znet_cmd_scene_dimming_duration_t;
typedef uint8_t znet_cmd_group_id_t;
typedef uint8_t znet_cmd_central_scene_value_t;

/**
 * @brief Scene Actuator Configuration report
 */
typedef struct znet_scene_actuator_configuration_report_t {
    znet_cmd_scene_id_t scene_id;                       /**< Current Scene Id */
    znet_cmd_scene_level_t level;                       /**< Current level */
    znet_cmd_scene_dimming_duration_t dimming_duration; /**< Current dimming duration */

} znet_scene_actuator_configuration_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_SCENE_ACTUATOR_CONFIGURATION_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    znet_cmd_scene_id_t scene_id,
    const znet_scene_actuator_configuration_report_t* value,
    void* arg );

/**
 * @brief Scene Controller Configuration report
 */
typedef struct znet_scene_controller_configuration_report_t {
    uint8_t __ver;                                      /**< reserved */
    znet_cmd_group_id_t group_id;                       /**< Current active grouping identifier */
    znet_cmd_scene_id_t scene_id;                       /**< Scene Id for the specified grouping identifier*/
    znet_cmd_scene_dimming_duration_t dimming_duration; /**< Current dimming duration */

} znet_scene_controller_configuration_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_SCENE_CONTROLLER_CONFIGURATION_RESULT )(
    znet_error_t err,
    znet_node_channel_id_t channelId,
    znet_node_id_t node_id,
    const znet_scene_controller_configuration_report_t* value,
    void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CENTRAL_SCENE_CONFIGURATION_RESULT )( znet_error_t err,
                                                                    znet_node_id_t node_id,
                                                                    znet_node_channel_id_t channel_id,
                                                                    znet_cmd_central_scene_value_t value,
                                                                    void* arg );

/**
 * @brief Central Scene Supported report
 */
typedef struct znet_central_scene_supported_report_t {
    uint8_t scenes_num;   /**< Max number of supported scenes*/
    uint8_t data_size;    /**< Size of Supported Key Attributes(1...3) */
    uint8_t identical;    /**< Advertise the supported Key Attributes for all or individual scenes */
    uint8_t slow_refresh; /**< Indicates whether the node supports the Slow Refresh capability. */
    const uint8_t* data;  /**< Key Attributes' Data */
} znet_central_scene_supported_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CENTRAL_SCENE_SUPPORTED_RESULT )( znet_error_t err,
                                                                znet_node_id_t node_id,
                                                                znet_node_channel_id_t channel_id,
                                                                const znet_central_scene_supported_report_t* value,
                                                                void* arg );

/**
 * @brief KeyAttribute advertise the unit supported.
 */
#define ZNET_KEY_ONE_TIME 0x01
#define ZNET_KEY_RELEASED 0x02
#define ZNET_KEY_HELD_DOWN 0x04
#define ZNET_KEY_PRESSED_TWO_TIMES 0x08
#define ZNET_KEY_PRESSED_THREE_TIMES 0x10
#define ZNET_KEY_PRESSED_FOUR_TIMES 0x20
#define ZNET_KEY_PRESSED_FIVE_TIMES 0x40
#define ZNET_KEY_SHORT_PRESSED 0x79

/**
 * @brief KeyAttribute Value advertise the unit notified.
 */
#define ZNET_MAX_REMOTE_BUTTONS 0x08
#define ZNET_KEY_PRESSED_ONE_TIME_VAL 0x00
#define ZNET_KEY_RELEASED_VAL 0x01
#define ZNET_KEY_PRESSED_HELD_DOWN_VAL 0x02
#define ZNET_KEY_PRESSED_TWO_TIMES_VAL 0x03
#define ZNET_KEY_PRESSED_THREE_TIMES_VAL 0x04
#define ZNET_KEY_PRESSED_FOUR_TIMES_VAL 0x05
#define ZNET_KEY_PRESSED_FIVE_TIMES_VAL 0x06

/**
 * @brief ButtonEvent advertise the unit button event.
 */
#define ZNET_REMOTE_BUTTON_1_SHORT_PRESS 0x01
#define ZNET_REMOTE_BUTTON_1_LONG_PRESS 0x02
#define ZNET_REMOTE_BUTTON_2_SHORT_PRESS 0x03
#define ZNET_REMOTE_BUTTON_2_LONG_PRESS 0x04
#define ZNET_REMOTE_BUTTON_3_SHORT_PRESS 0x05
#define ZNET_REMOTE_BUTTON_3_LONG_PRESS 0x06
#define ZNET_REMOTE_BUTTON_4_SHORT_PRESS 0x07
#define ZNET_REMOTE_BUTTON_4_LONG_PRESS 0x08
#define ZNET_REMOTE_BUTTON_5_SHORT_PRESS 0x09
#define ZNET_REMOTE_BUTTON_5_LONG_PRESS 0x0A
#define ZNET_REMOTE_BUTTON_6_SHORT_PRESS 0x0B
#define ZNET_REMOTE_BUTTON_6_LONG_PRESS 0x0C
#define ZNET_REMOTE_BUTTON_7_SHORT_PRESS 0x0D
#define ZNET_REMOTE_BUTTON_7_LONG_PRESS 0x0E
#define ZNET_REMOTE_BUTTON_8_SHORT_PRESS 0x0F
#define ZNET_REMOTE_BUTTON_8_LONG_PRESS 0x10

#define ZNET_KEY_ATTRIB_MASK 0x07
#define ZNET_SLOW_REFRESH_MASK 0x80
/**
 * @brief Central Scene Notification
 */
#define ZNET_CMD_CS_NOTIFICATION_CHECK_LEN 0x05
typedef struct znet_central_scene_notification_t {
    uint8_t sequence_number; /**< Sequence number */
    uint8_t slow_refresh;    /**< Indicates whether the node supports the Slow Refresh capability. */
    uint8_t key_attribute;   /**< Advertise the event detected by key */
    uint8_t scene_number;    /**< Advertises number of supported scene */
} znet_central_scene_notification_t;
/**
 * @brief Function prototype for notify: Central Scene.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_CENTRAL_SCENE_NOTIFICATION )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            const znet_central_scene_notification_t* value,
                                                            void* arg );

typedef uint8_t znet_scene_activation_t;
/**
 * @brief Function prototype for notify: Scene Activation.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param sceneNumber Scene number
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_SCENE_ACTIVATION_SET_RESULT )( znet_error_t err,
                                                             znet_node_id_t nodeId,
                                                             const znet_scene_activation_t sceneNumber,
                                                             void* arg );

/**
 * @brief Function prototype for notify: remove node complated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_DEVICE_RESET_LOCALLY )( znet_error_t err, const znet_node_id_t node_info, void* arg );

/**
 * @brief Inclution Controller value
 */
typedef uint8_t znet_inclution_controller_step_id_t;
typedef uint8_t znet_inclution_controller_status_t;

#define ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION 0x01
#define ZNET_INCLUSION_CONTROLLER_S0_INCLUSION 0x02
#define ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION_REPLACE 0x03

#define ZNET_INCLUSION_CONTROLLER_STEP_OK 0x01
#define ZNET_INCLUSION_CONTROLLER_STEP_USER_REJECTED 0x02
#define ZNET_INCLUSION_CONTROLLER_STEP_FAILED 0x03
#define ZNET_INCLUSION_CONTROLLER_STEP_NOT_SUPPORTED 0x04

#define ZNET_INCLUSION_INITIATE 0x01
#define ZNET_INCLUSION_COMPLETE 0x02

/**
 * @brief Function prototype for notify: inclusion controller initiated
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_INCLUSION_CONTROLLER_INITIATE )( znet_error_t err,
                                                               const znet_node_id_t node_id,
                                                               const znet_node_id_t child_node_id,
                                                               const znet_inclution_controller_step_id_t step_id,
                                                               void* arg );

/**
 * @brief Function prototype for notify: inclusion controller completed
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_INCLUSION_CONTROLLER_COMPLETE )( znet_error_t err,
                                                               const znet_node_id_t node_id,
                                                               const znet_inclution_controller_step_id_t step_id,
                                                               const znet_inclution_controller_status_t status,
                                                               void* arg );

/**
 * @brief Node Provisioning Set
 */
typedef struct znet_node_provisioning_set_t {
    uint8_t sequence_number; /**< Commands classes count */
    uint8_t dsk_key[16];
    uint8_t meta_data_length;
    const uint8_t* meta_data;
} znet_node_provisioning_set_t;

/**
 * @brief Function prototype for notify: node provisioning set received
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NODE_PROVISIONING_SET )( znet_error_t err,
                                                       const znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       const znet_node_provisioning_set_t* value,
                                                       void* arg );

/**
 * @brief Node Provisioning Delete
 */
typedef struct znet_node_provisioning_delete_t {
    uint8_t sequence_number; /**< Commands classes count */
    bool delete_all;         /**< Commands classes count */
    uint8_t dsk_key[16];
} znet_node_provisioning_delete_t;

/**
 * @brief Function prototype for notify: node provisioning delete received
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NODE_PROVISIONING_DELETE )( znet_error_t err,
                                                          const znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          const znet_node_provisioning_delete_t* value,
                                                          void* arg );

/**
 * @brief Function prototype for notify: node provisioning get received
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NODE_PROVISIONING_GET )( znet_error_t err,
                                                       const znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       uint8_t sequence_number,
                                                       const uint8_t dsk_key[16],
                                                       void* arg );

/**
 * @brief Function prototype for notify: node provisioning list iteration get received
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_NODE_PROVISIONING_LIST_ITERATION_GET )( znet_error_t err,
                                                                      const znet_node_id_t node_id,
                                                                      znet_node_channel_id_t channel_id,
                                                                      uint8_t sequence_number,
                                                                      uint8_t remaining_counter,
                                                                      void* arg );

/**
 * @brief Security and security2 support report
 */
typedef struct znet_security_supported_report_t {
    uint8_t commands_count;          /**< Commands classes count */
    znet_command_class_t commands[]; /**< Commands classes */

} znet_security_supported_report_t;

/**
 * @brief Function prototype for notify: node cmd security or security2 support
 * report/result
 *
 * It is called many times. value is NULL if
 * there are no more reports.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_SECURITY_SUPPORTED_RESULT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           const znet_security_supported_report_t* value,
                                                           void* arg );

/**
 * @brief Grouping Identifier
 */
typedef uint8_t znet_grouping_id_t;

/**
 * @brief Destinations of a given association group.
 */
typedef struct znet_association_report_t {
    znet_grouping_id_t grouping_id; /**< Grouping Identifier */
    uint8_t max_nodes_supported;    /**< Max Nodes Supported */
    uint8_t node_id_count;          /**< Node ID count */
    znet_node_id_t node_ids[];      /**< Node ID */
} znet_association_report_t;

/**
 * @brief TBD.
 *
 * It is called many times. value is NULL if
 * there are no more reports.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_RESULT )( znet_error_t err,
                                                    znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_grouping_id_t grouping_id,
                                                    const znet_association_report_t* value,
                                                    void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_SET_DONE )( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      znet_grouping_id_t grouping_id,
                                                      void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_REMOVE_DONE )( znet_error_t err,
                                                         znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         void* arg );

/**
 * @brief Advertise the maximum number of association groups implemented by
 * node.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value (Supported groupings count)
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_SUPPORTED_GROUPING_RESULT )( znet_error_t err,
                                                                       znet_node_id_t node_id,
                                                                       znet_node_channel_id_t channelId,
                                                                       uint8_t value,
                                                                       void* arg );

/**
 * @brief Advertise the association group that represents the most recently
 * detected button.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_SPECIFIC_GROUP_RESULT )( znet_error_t err,
                                                                   znet_node_id_t node_id,
                                                                   znet_grouping_id_t value,
                                                                   void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_association_group_name_report_t {
    znet_grouping_id_t grouping_id; /**< Grouping Identifier */
    uint8_t name_size;              /**< Group name size (without NULL) */
    char name[];                    /**< Group name (UTF-8, NULL terminated */
} znet_association_group_name_report_t;

/**
 * @brief Advertise the assigned name of an association group.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_GROUP_NAME_RESULT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channelId,
                                                               znet_grouping_id_t grouping_id,
                                                               const znet_association_group_name_report_t* value,
                                                               void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_association_group_info_report_t {
    uint8_t list_mode;        /**< List mode */
    uint8_t dynamic_info;     /**< Dynamic_info */
    uint8_t group_info_count; /**< Commands count */
    struct {
        znet_grouping_id_t grouping_id; /**< Grouping Identifier */
        uint16_t profile;               /**< Profile */
    } group_infos[];                    /**< Group Info */
} znet_association_group_info_report_t;

/**
 * @brief Advertise the properties of one or more association groups.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_GROUP_INFO_RESULT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channelId,
                                                               znet_grouping_id_t grouping_id,
                                                               const znet_association_group_info_report_t* value,
                                                               void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_association_group_command_list_report_t {
    znet_grouping_id_t grouping_id; /**< Grouping Identifier */
    uint8_t commands_count;         /**< Commands count */
    struct {
        znet_command_class_t command; /**< Command class */
        uint8_t sub_command;          /**< Command in command class */
    } commands[];                     /**< Commands */
} znet_association_group_command_list_report_t;
/**
 * @brief Advertise the commands that are sent via an actual association group.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_ASSOCIATION_GROUP_COMMAND_LIST_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channelId,
    znet_grouping_id_t grouping_id,
    const znet_association_group_command_list_report_t* value,
    void* arg );

/**
 * @brief Destinations of a given multi channel association group.
 */
typedef struct znet_multichannel_association_report_t {
    uint8_t __ver;                   /**< reserved */
    znet_grouping_id_t grouping_id;  /**< Grouping Identifier */
    uint8_t max_end_pints_supported; /**< Max endpoints Supported */
    uint8_t end_pints_count;         /**< End points count */
    znet_endpoint_t end_pints[];     /**< End points */
} znet_multichannel_association_report_t;

/**
 * @brief TBD.
 *
 * It is called many times. value is NULL if
 * there are no more reports.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_RESULT )( znet_error_t err,
                                                                 znet_node_id_t node_id,
                                                                 znet_node_channel_id_t channelId,
                                                                 znet_grouping_id_t groupingId,
                                                                 const znet_multichannel_association_report_t* value,
                                                                 void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_SET_DONE )( znet_error_t err,
                                                                   znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   znet_grouping_id_t grouping_id,
                                                                   void* arg );

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_REMOVE_DONE )( znet_error_t err,
                                                                      znet_node_id_t node_id,
                                                                      znet_node_channel_id_t channel_id,
                                                                      void* arg );

/**
 * @brief Advertise the maximum number of multi channel association groups
 * implemented by node.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value (Supported groupings count)
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_SUPPORTED_GROUPING_RESULT )( znet_error_t err,
                                                                                    znet_node_id_t node_id,
                                                                                    znet_node_channel_id_t channelId,
                                                                                    uint8_t value,
                                                                                    void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_simple_av_set_report_t {
    uint8_t sequence_id;   /**< Swquence Identifier */
    uint8_t key_attribute; /**< Key attribute */
    uint8_t commands_count;
    uint16_t commands[]; /**< Commands */
} znet_simple_av_set_report_t;

typedef void ( *ZNET_NODE_CMD_SIMPLE_AV_CONTROL_SET_RESULT )( znet_error_t err,
                                                              znet_node_id_t node_id,
                                                              znet_node_channel_id_t channelId,
                                                              const znet_simple_av_set_report_t* value,
                                                              void* arg );

/**
 * @brief Function prototype for notify: No operation result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NO_OPERATION_RESULT )( znet_error_t err, znet_node_id_t node_id, void* arg );
typedef uint32_t znet_cmd_wakeup_value_t;

#define ZNET_CMD_WAKEUP_REPORT_CHECK_LEN 0x06

/**
 * @brief Wake up: Parameter types
 */
typedef struct znet_wakeup_report_t {
    znet_cmd_wakeup_value_t value; /**< wakeup interval*/
    znet_home_id_t nodeId;         /**< node Id */
} znet_wakeup_report_t;

/**
 * @brief Function prototype for : Wake Up.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_WAKEUP_RESULT )( znet_error_t err,
                                               znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               znet_cmd_wakeup_value_t value,
                                               znet_node_id_t target_node_id,
                                               void* arg );

#define ZNET_CMD_WAKEUP_NOTIFICATION_CHECK_LEN 0x02

/**
 * @brief Function prototype for notify: Wake Up.
 *
 * Automatically call "znet_node_cmd_wakeup_sleep" command after this callback
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_WAKEUP_NOTIFICATION )( znet_error_t err, znet_node_id_t node_id, void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_wakeup_capabilities_report_t {
    uint32_t min_wakeup_interval;     /***/
    uint32_t max_wakeup_interval;     /***/
    uint32_t default_wakeup_interval; /***/
    uint32_t step_wakeup_interval;    /***/

} znet_wakeup_capabilities_report_t;

#define ZNET_CMD_WAKEUP_CAPABILITIES_REPORT_CHECK_LEN 0x0E

/**
 * @brief Function prototype for : Wake Up.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_WAKEUP_CAPABILITIES_RESULT )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            const znet_wakeup_capabilities_report_t* value,
                                                            void* arg );

/**
 * @brief Mode advertise the unit used.
 */
#define ZNET_THERMOSTAT_MODE_OFF 0x00
#define ZNET_THERMOSTAT_MODE_HEAT 0x01
#define ZNET_THERMOSTAT_MODE_COOL 0x02
#define ZNET_THERMOSTAT_MODE_AUTO 0x03
#define ZNET_THERMOSTAT_MODE_AUXILIARY_HEAT 0x04
#define ZNET_THERMOSTAT_MODE_RESUME 0x05
#define ZNET_THERMOSTAT_MODE_FAN_ONLY 0x06
#define ZNET_THERMOSTAT_MODE_FURNACE 0x07
#define ZNET_THERMOSTAT_MODE_DRY_AIR 0x08
#define ZNET_THERMOSTAT_MODE_MOIST_AIR 0x09
#define ZNET_THERMOSTAT_MODE_AUTO_CHANGEOVER 0x0A
#define ZNET_THERMOSTAT_MODE_ENERGY_SAVE_HEAT 0x0B
#define ZNET_THERMOSTAT_MODE_ENERGY_SAVE_COOL 0x0C
#define ZNET_THERMOSTAT_MODE_AWAY 0x0D
#define ZNET_THERMOSTAT_MODE_RESERVED 0x0E
#define ZNET_THERMOSTAT_MODE_FULL_POWER 0x0F
#define ZNET_THERMOSTAT_MODE_RESERVED0 0x10
#define ZNET_THERMOSTAT_MODE_RESERVED1 0x11
#define ZNET_THERMOSTAT_MODE_RESERVED2 0x12
#define ZNET_THERMOSTAT_MODE_RESERVED3 0x13
#define ZNET_THERMOSTAT_MODE_RESERVED4 0x14
#define ZNET_THERMOSTAT_MODE_RESERVED5 0x15
#define ZNET_THERMOSTAT_MODE_RESERVED6 0x16
#define ZNET_THERMOSTAT_MODE_RESERVED7 0x17
#define ZNET_THERMOSTAT_MODE_RESERVED8 0x18
#define ZNET_THERMOSTAT_MODE_RESERVED9 0x19
#define ZNET_THERMOSTAT_MODE_RESERVEDA 0x1A
#define ZNET_THERMOSTAT_MODE_RESERVEDB 0x1B
#define ZNET_THERMOSTAT_MODE_RESERVEDC 0x1C
#define ZNET_THERMOSTAT_MODE_RESERVEDD 0x1D
#define ZNET_THERMOSTAT_MODE_RESERVEDE 0x1E
#define ZNET_THERMOSTAT_MODE_MANUFACTURER_SPECIFC 0x1F
#define ZNET_THERMOSTAT_MODE_MANUFACTURER_DATA_FIELDS_MASK 0xE0
#define ZNET_THERMOSTAT_MODE_MANUFACTURER_DATA_FIELDS_SHIFT 0x05

typedef uint8_t znet_cmd_thermostat_mode_value_t;

/**
 * @brief Thermostat mode report
 */
typedef struct znet_thermostat_mode_report_t {
    uint8_t mode;         /**< Thermostat mode */
    uint8_t dcount;       /**< Data count */
    uint8_t manuf_data[]; /**< Manufacturer Data fields */
} znet_thermostat_mode_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_MODE_RESULT )( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        const znet_thermostat_mode_report_t* value,
                                                        void* arg );

/**
 * @brief Thermostat mode supported report
 */
typedef struct znet_thermostat_mode_supported_report_t {
    uint8_t mcount;          /**< Masks count */
    const uint8_t* bit_mask; /**< Bit Mask fields describe the supported modes */
} znet_thermostat_mode_supported_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_MODE_SUPPORTED_RESULT )( znet_error_t err,
                                                                  znet_node_id_t node_id,
                                                                  znet_node_channel_id_t channel_id,
                                                                  const znet_thermostat_mode_supported_report_t* value,
                                                                  void* arg );

/**
 * @brief Operating state advertise the unit used.
 */
#define ZNET_THERMOSTAT_OPERATING_STATE_IDLE 0x00
#define ZNET_THERMOSTAT_OPERATING_STATE_HEATING 0x01
#define ZNET_THERMOSTAT_OPERATING_STATE_COOLING 0x02
#define ZNET_THERMOSTAT_OPERATING_STATE_FAN_ONLY 0x03
#define ZNET_THERMOSTAT_OPERATING_STATE_PENDING_HEAT 0x04
#define ZNET_THERMOSTAT_OPERATING_STATE_PENDING_COOL 0x05
#define ZNET_THERMOSTAT_OPERATING_STATE_VENT_ECO 0x06
#define ZNET_THERMOSTAT_OPERATING_STATE_AUX_HEATING 0x07
#define ZNET_THERMOSTAT_OPERATING_STATE_SEC_STAGE_HEATING 0x08
#define ZNET_THERMOSTAT_OPERATING_STATE_SEC_STAGE_COOLING 0x09
#define ZNET_THERMOSTAT_OPERATING_STATE_SEC_STAGE_AUX_HEAT 0x0A
#define ZNET_THERMOSTAT_OPERATING_STATE_THIRD_STAGE_AUX_HEAT 0x0B
#define ZNET_THERMOSTAT_OPERATING_STATE_MAX_VALUE 0x0B

typedef uint8_t znet_cmd_thermostat_operating_state_value_t;

/**
 * @brief Thermostat operating state result/report
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Operating state
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_OPERATING_STATE_RESULT )( znet_error_t err,
                                                                   znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   znet_cmd_thermostat_operating_state_value_t value,
                                                                   void* arg );

/**
 * @brief Thermostat operating state logging supported report
 */
typedef struct znet_thermostat_operating_state_logging_supported_report_t {
    uint8_t opscount;        /**< Masks count */
    const uint8_t* bit_mask; /**< Bit Mask fields describe the supported modes */
} znet_thermostat_operating_state_logging_supported_report_t;

/**
 * @brief Thermostat fan state result/report
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_OPERATING_STATE_LOGGING_SUPPORTED_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_thermostat_operating_state_logging_supported_report_t* value,
    void* arg );

/**
 * @brief Thermostat operating state logging report
 */
typedef struct znet_thermostat_operating_state_logging_report_t {
    uint8_t ver;         /**< reserved */
    uint16_t data_count; /**< Size of the all received logging frames (in bytes)*/
    uint8_t data[];      /**< Configuration Data */
} znet_thermostat_operating_state_logging_report_t;

/**
 * @brief TBD.
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_OPERATING_STATE_LOGGING_RESULT )(
    int err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    znet_thermostat_operating_state_logging_report_t* value,
    void* arg );

/**
 * @brief Fan Mode advertise the unit used.
 */
#define ZNET_THERMOSTAT_FAN_MODE_AUTO_LOW 0x00
#define ZNET_THERMOSTAT_FAN_MODE_LOW 0x01
#define ZNET_THERMOSTAT_FAN_MODE_AUTO_HIGH 0x02
#define ZNET_THERMOSTAT_FAN_MODE_HIGH 0x03
#define ZNET_THERMOSTAT_FAN_MODE_AUTO_MEDIUM 0x04
#define ZNET_THERMOSTAT_FAN_MODE_MEDIUM 0x05
#define ZNET_THERMOSTAT_FAN_MODE_CIRCULATION 0x06
#define ZNET_THERMOSTAT_FAN_MODE_HUMIDITY_CIRCULATION 0x07
#define ZNET_THERMOSTAT_FAN_MODE_LEFT_AND_RIGHT 0x08
#define ZNET_THERMOSTAT_FAN_MODE_UP_AND_DOWN 0x09
#define ZNET_THERMOSTAT_FAN_MODE_QUIET 0x0A
#define ZNET_THERMOSTAT_FAN_MODE_RESERVEDB 0x0B
#define ZNET_THERMOSTAT_FAN_MODE_RESERVEDC 0x0C
#define ZNET_THERMOSTAT_FAN_MODE_RESERVEDD 0x0D
#define ZNET_THERMOSTAT_FAN_MODE_RESERVEDE 0x0E
#define ZNET_THERMOSTAT_FAN_MODE_RESERVEDF 0x0F
#define ZNET_THERMOSTAT_FAN_MODE_MAX_VALUE 0x0A

typedef uint8_t znet_cmd_thermostat_fan_mode_value_t;

/**
 * @brief Thermostat fan mode result/report
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Fan mode
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_FAN_MODE_RESULT )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            znet_cmd_thermostat_fan_mode_value_t value,
                                                            void* arg );

/**
 * @brief Thermostat Fan mode supported report
 */
typedef struct znet_thermostat_fan_mode_supported_report_t {
    uint8_t fmcount;         /**< Masks count */
    const uint8_t* bit_mask; /**< Bit Mask fields describe the supported fan modes */
} znet_thermostat_fan_mode_supported_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_FAN_MODE_SUPPORTED_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_thermostat_fan_mode_supported_report_t* value,
    void* arg );

/**
 * @brief Fan State advertise the unit used.
 */
#define ZNET_THERMOSTAT_FAN_STATE_IDLE_OFF 0x00
#define ZNET_THERMOSTAT_FAN_STATE_RUNNING_LOW 0x01
#define ZNET_THERMOSTAT_FAN_STATE_RUNNING_HIGH 0x02
#define ZNET_THERMOSTAT_FAN_STATE_RUNNING_MEDIUM 0x03
#define ZNET_THERMOSTAT_FAN_STATE_CIRCULATION_MODE 0x04
#define ZNET_THERMOSTAT_FAN_STATE_HUMIDITY_CIRCULATION_MODE 0x05
#define ZNET_THERMOSTAT_FAN_STATE_RIGHT_LEFT_CIRCULATION_MODE 0x06
#define ZNET_THERMOSTAT_FAN_STATE_UP_DOWN_CIRCULATION_MODE 0x07
#define ZNET_THERMOSTAT_FAN_STATE_QUIET_CIRCULATION_MODE 0x08
#define ZNET_THERMOSTAT_FAN_STATE_MAX_VALUE 0x08
typedef uint8_t znet_cmd_thermostat_fan_state_value_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Fan state
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_FAN_STATE_RESULT )( znet_error_t err,
                                                             znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             znet_cmd_thermostat_fan_state_value_t value,
                                                             void* arg );
/**
 * @brief Thermostat setback result/report
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_SETBACK_RESULT )( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           uint8_t type,
                                                           int8_t state,
                                                           void* arg );

/**
 * @brief SetPoint advertise the unit used.
 */
#define ZNET_THERMOSTAT_SETPOINT_ALL_SUPPORTED 0x0
#define ZNET_THERMOSTAT_SETPOINT_HEATING 0x01
#define ZNET_THERMOSTAT_SETPOINT_COOLING 0x02
#define ZNET_THERMOSTAT_SETPOINT_RESERVED3 0x03
#define ZNET_THERMOSTAT_SETPOINT_RESERVED4 0x04
#define ZNET_THERMOSTAT_SETPOINT_RESERVED5 0x05
#define ZNET_THERMOSTAT_SETPOINT_RESERVED6 0x06
#define ZNET_THERMOSTAT_SETPOINT_FURNACE 0x07
#define ZNET_THERMOSTAT_SETPOINT_DRY_AIR 0x08
#define ZNET_THERMOSTAT_SETPOINT_MOIST_AIR 0x09
#define ZNET_THERMOSTAT_SETPOINT_AUTO_CHANGEOVER 0x0A
#define ZNET_THERMOSTAT_SETPOINT_ENERGY_SAVE_HEATING 0x0B
#define ZNET_THERMOSTAT_SETPOINT_ENERGY_SAVE_COOLING 0x0C
#define ZNET_THERMOSTAT_SETPOINT_AWAY_HEATING 0x0D
#define ZNET_THERMOSTAT_SETPOINT_AWAY_COOLING 0x0E
#define ZNET_THERMOSTAT_SETPOINT_FULL_POWER 0x0F
#define ZNET_THERMOSTAT_SETPOINT_VER3_MAX_TYPE 0x10

typedef uint8_t znet_cmd_thermostat_setpoint_value_t;

/**
 * @brief Thermostat Setpoint supported report
 */
typedef struct znet_thermostat_setpoint_report_t {
    uint8_t type;
    uint8_t scale;
    uint8_t size;
    uint8_t precision;
    int32_t sp_value;
} znet_thermostat_setpoint_report_t;
/**
 * @brief Thermostat Setpoint result
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Setpoint value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_SETPOINT_RESULT )( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            uint8_t type,
                                                            const znet_thermostat_setpoint_report_t* value,
                                                            void* arg );

/**
 * @brief Thermostat Setpoint supported report
 */
typedef struct znet_thermostat_setpoint_supported_report_t {
    uint8_t stcount;         /**< Masks count */
    const uint8_t* bit_mask; /**< Bit Mask fields describe the supported fan modes */
} znet_thermostat_setpoint_supported_report_t;

/**
 * @brief TBD.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Current value
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_SETPOINT_SUPPORTED_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_thermostat_setpoint_supported_report_t* value,
    void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_thermostat_setpoint_capabilities_report_t {
    uint8_t type;
    uint8_t min_params;
    uint32_t min_value;
    uint8_t max_params;
    uint32_t max_value;
} znet_thermostat_setpoint_capabilities_report_t;

#define ZNET_CMD_WAKEUP_CAPABILITIES_REPORT_CHECK_LEN 0x0E

/**
 * @brief Function prototype for : Wake Up.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_NODE_CMD_THERMOSTAT_SETPOINT_CAPABILITIES_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    uint8_t type,
    const znet_thermostat_setpoint_capabilities_report_t* value,
    void* arg );

/**
 * @brief Function prototype for : Zwave Plus Info Get.
 *
 */
typedef void ( *ZNET_NODE_ZWAVE_PLUS_INFO_GET )( void* arg );

/**
 * @brief Function prototype for : Indicator Identify.
 *
 */
typedef void ( *ZNET_INDICATOR_IDENTIFY )( void* arg );

/**
 * @brief Indicator property state
 */
typedef struct znet_indicator_property_t {
    uint8_t indicator_id; /**< Indicator id */
    uint8_t property_id;  /**< Property id */
    uint8_t value;        /**< Value */
} znet_indicator_property_t;

/**
 * @brief Indicator state
 */
typedef struct znet_indicator_report_t {
    uint8_t indicator_0_value;              /**< Indicator 0 value */
    uint8_t indicator_objects_count;        /**< Indicator objects count */
    znet_indicator_property_t properties[]; /**< Indicator properties */
} znet_indicator_report_t;

/**
 * @brief Function prototype for Indicator Report.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Report
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_INDICATOR_RESULT )( znet_error_t err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         uint8_t indicator_id,
                                         const znet_indicator_report_t* value,
                                         void* arg );

/**
 * @brief Indicator Supported
 */
typedef struct znet_indicator_supported_report_t {
    uint8_t indicator_id;       /**< Indicator id */
    uint8_t next_indicator_id;  /**< Next indicator id */
    uint8_t supported_mask_len; /**< Bit mask len */
    uint8_t bit_mask[];         /**< Bit mask */
} znet_indicator_supported_report_t;

/**
 * @brief Function prototype for Indicator Report.
 *
 * @param err Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Report
 * @param arg Parameter for callback functions
 */
typedef void ( *ZNET_INDICATOR_SUPPORTED_RESULT )( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t indicator_id,
                                                   const znet_indicator_supported_report_t* value,
                                                   void* arg );

/**
 * @brief Function prototype for reading private DSK.
 *
 * @param err Error code
 * @param dsk_key Vector with DSK
 * @param size dsk_key vector size
 */
typedef void ( *ZNET_PRIVATE_S2_DSK_RESULT )( znet_error_t err, const char* dsk_key, size_t size );

/**
 * @brief Function prototype for request node`s security
 *
 * @param node_id Node ID security we want to know
 * @return Return security
 */
typedef uint8_t ( *ZNET_NODE_SECURITY_GET )( znet_node_id_t node_id, void* arg );

/**
 * @brief Function prototype for checking is node added to network
 *
 * @param node_id Node ID security we want to know
 * @return Return security
 */
typedef znet_error_t ( *ZNET_NODE_IS_ADDED )( znet_node_id_t node_id, void* arg );

/**
 * @brief Function prototype for request node`s security
 *
 * @param node_id Node ID security we want to set
 * @param security level for Node ID
 * @return Return zero on success. On error, -1 is returned
 */
typedef int ( *ZNET_NODE_SECURITY_SET )( znet_node_id_t node_id, uint8_t security, void* arg );

/**
 * @brief Function prototype for request supported CC
 *
 * @param node_id Node ID
 * @param command Command class ID
 * @return Return security
 */
typedef znet_error_t ( *ZNET_NODE_IS_SUPPORTED_CC )( znet_node_id_t node_id, znet_command_class_t command, void* arg );

/**
 * @brief Function prototype for request is node wakeup
 *
 * @param node_id Node ID
 * @return Return security
 */
typedef int ( *ZNET_NODE_IS_WAKEUP )( znet_node_id_t node_id, void* arg );

typedef znet_date_t znet_local_date_t;
typedef znet_time_t znet_local_time_t;

typedef struct znet_dst_time_point_t {
    uint8_t month;
    uint8_t day;
    uint8_t hour;
} znet_dst_time_point_t;

typedef struct znet_time_zone_t {
    uint8_t is_tzo_negative : 1;
    uint8_t hour_tzo : 7;
    uint8_t minute_tzo;
} znet_time_zone_t;

static inline bool is_time_zone_valid( const znet_time_zone_t* tz ) {
    return tz->hour_tzo <= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MAX &&
           /*tz->minute_tzo >= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MIN &&*/
           tz->minute_tzo <= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MAX;
}

typedef struct znet_time_zone_settings_t {
    znet_time_zone_t time_zone;

    int8_t minute_dst_offset;
    znet_dst_time_point_t start_dst;
    znet_dst_time_point_t end_dst;
} znet_time_zone_settings_t;

typedef void ( *ZNET_NODE_TIME_REQUESTED )( znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            uint8_t encap,
                                            void* arg );

typedef void ( *ZNET_NODE_DATE_REQUESTED )( znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            uint8_t encap,
                                            void* arg );

typedef void ( *ZNET_NODE_TIME_ZONE_REQUESTED )( znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 uint8_t encap,
                                                 void* arg );

typedef void ( *ZNET_NODE_TIME_ZONE_RESULT )( znet_error_t err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              const znet_time_zone_settings_t* time_zone_settings,
                                              void* arg );

typedef void ( *ZNET_NODE_TIME_ZONE_SET )( znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           const znet_time_zone_settings_t* time_zone_settings,
                                           void* arg );

typedef void ( *ZNET_NODE_LOCAL_DATE_RESULT )( znet_error_t err,
                                               znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               const znet_local_date_t* local_date,
                                               void* arg );

typedef void ( *ZNET_NODE_LOCAL_TIME_RESULT )( znet_error_t err,
                                               znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               const znet_local_time_t* local_time,
                                               void* arg );

#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_YEAR_MIN 0
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_YEAR_MAX 99
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_MONTH_MIN 1
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_MONTH_MAX 12
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_DAY_MIN 1
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_DAY_MAX 31
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_HOUR_MIN 0
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_HOUR_MAX 23
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_MINUTE_MIN 0
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_MINUTE_MAX 59
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_WEEK_DAY_MIN 0
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_WEEK_DAY_MAX 6
#define ZNET_CMD_SCHEDULE_ENTRY_LOCK_ALL_WEEK_DAYS_MASK \
    ( ( 1 << ( ZNET_CMD_SCHEDULE_ENTRY_LOCK_WEEK_DAY_MAX + 1 ) ) - 1 )

typedef struct znet_schedule_id_t {
    uint8_t user_id;
    uint8_t slot_id;
} znet_schedule_id_t;

typedef struct znet_week_day_schedule_t {
    znet_schedule_id_t schedule_id;
    uint8_t day_of_week;
    uint8_t start_hour;
    uint8_t start_min;
    uint8_t stop_hour;
    uint8_t stop_min;
} znet_week_day_schedule_t;

typedef struct znet_year_day_schedule_t {
    znet_schedule_id_t schedule_id;
    uint8_t start_year;
    uint8_t start_month;
    uint8_t start_day;
    uint8_t start_hour;
    uint8_t start_minute;
    uint8_t stop_year;
    uint8_t stop_month;
    uint8_t stop_day;
    uint8_t stop_hour;
    uint8_t stop_minute;
} znet_year_day_schedule_t;

typedef struct znet_cmd_schedule_entry_lock_supported_report_t {
    uint8_t week_day_slots;
    uint8_t year_day_slots;
    uint8_t daily_repeating_slots;
} znet_cmd_schedule_entry_lock_supported_report_t;

typedef struct znet_daily_repeating_schedule_t {
    znet_schedule_id_t schedule_id;
    uint8_t week_day_bitmask;
    uint8_t start_hour;
    uint8_t start_min;
    uint8_t duration_hour;
    uint8_t duration_min;
} znet_daily_repeating_schedule_t;

typedef struct znet_cmd_schedule_entry_lock_time_offset_t {
    znet_time_zone_t time_zone;
    int8_t minute_dst_offset;
} znet_cmd_schedule_entry_lock_time_offset_t;

typedef void ( *ZNET_NODE_SCHEDULE_LOCK_ENTRY_TIME_OFFSET_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_cmd_schedule_entry_lock_time_offset_t* report,
    void* arg );

typedef void ( *ZNET_NODE_SCHEDULE_LOCK_ENTRY_WEEK_DAY_RESULT )( znet_error_t err,
                                                                 znet_node_id_t node_id,
                                                                 znet_node_channel_id_t channel_id,
                                                                 uint8_t user_id,
                                                                 uint8_t slot_id,
                                                                 const znet_week_day_schedule_t* report,
                                                                 void* arg );

typedef void ( *ZNET_NODE_SCHEDULE_LOCK_ENTRY_YEAR_DAY_RESULT )( znet_error_t err,
                                                                 znet_node_id_t node_id,
                                                                 znet_node_channel_id_t channel_id,
                                                                 uint8_t user_id,
                                                                 uint8_t slot_id,
                                                                 const znet_year_day_schedule_t* report,
                                                                 void* arg );

typedef void ( *ZNET_NODE_SCHEDULE_LOCK_ENTRY_SUPPORTED_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_cmd_schedule_entry_lock_supported_report_t* report,
    void* arg );

typedef void ( *ZNET_NODE_SCHEDULE_LOCK_ENTRY_DAILY_REPEATING_RESULT )( znet_error_t err,
                                                                        znet_node_id_t node_id,
                                                                        znet_node_channel_id_t channel_id,
                                                                        uint8_t user_id,
                                                                        uint8_t slot_id,
                                                                        const znet_daily_repeating_schedule_t* report,
                                                                        void* arg );

typedef struct znet_firmware_update_md_report_v1_t {
    bool is_last_report;
    uint8_t data_size;
    const uint8_t* data;
} znet_firmware_update_md_report_v1_t;

typedef struct znet_firmware_update_md_activation_status_report_v4_t {
    znet_firmware_info_t firmware;
    uint8_t status;
} znet_firmware_update_md_activation_status_report_v4_t;

typedef struct znet_firmware_update_md_activation_status_report_v5_t {
    uint8_t hardware_revision;
} znet_firmware_update_md_activation_status_report_v5_t;

typedef void ( *ZNET_NODE_FIRMWARE_MD_RESULT )( znet_error_t err,
                                                znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                const znet_firmware_md_report_v1_t* report_v1,
                                                const znet_firmware_md_report_v3_t* report_v3_opt,
                                                const znet_firmware_md_report_v5_t* report_v5_opt,
                                                void* arg );

typedef void ( *ZNET_NODE_FIRMWARE_UPDATE_MD_REQUEST_RESULT )( znet_error_t err,
                                                               znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               uint8_t status,
                                                               void* arg );

typedef void ( *ZNET_NODE_FIRMWARE_UPDATE_MD_REQUESTED )( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          uint8_t requested_reports_count,
                                                          uint16_t report_number,
                                                          void* arg );

typedef void ( *ZNET_NODE_FIRMWARE_UPDATE_MD_RESULT )( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       uint16_t report_number,
                                                       bool is_last_report,
                                                       uint8_t data_size,
                                                       const uint8_t* data,
                                                       void* arg );

typedef void ( *ZNET_NODE_FIRMWARE_UPDATE_MD_STATUS_RESULT )( znet_error_t err,
                                                              znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              uint8_t status,
                                                              uint16_t waiting_time,
                                                              void* arg );

typedef void ( *ZNET_NODE_FIRMWARE_UPDATE_MD_ACTIVATION_STATUS_RESULT )(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    uint8_t target,
    const znet_firmware_update_md_activation_status_report_v4_t* report_v4,
    const znet_firmware_update_md_activation_status_report_v5_t* report_v5_opt,
    void* arg );

typedef void ( *ZNET_NODE_FIRMWARE_UPDATE_MD_PREPARATION_RESULT )( znet_error_t err,
                                                                   znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   uint8_t status,
                                                                   uint16_t firmware_checksum,
                                                                   void* arg );

typedef enum znet_backup_status_t {
    ZNET_BACKUP_STATUS_OK = 0,
    ZNET_BACKUP_STATUS_ERROR = 1,
    ZNET_BACKUP_STATUS_OPERATION_MISHMATCH_ERROR = 2,
    ZNET_BACKUP_STATUS_OPERATION_DISTURBED_ERROR = 3,
    ZNET_BACKUP_STATUS_END_OF_FILE = 0xFF
} znet_backup_status_t;

typedef void ( *ZNET_BACKUP_SESSION_OPEN_RESULT )( znet_error_t err,
                                                   znet_backup_status_t status,
                                                   uint16_t size,
                                                   void* arg );
typedef void ( *ZNET_BACKUP_SESSION_CLOSE_RESULT )( znet_error_t err, znet_backup_status_t status, void* arg );

typedef void ( *ZNET_BACKUP_READ_RESULT )( znet_error_t err,
                                           uint16_t requested_offset,
                                           znet_backup_status_t status,
                                           uint16_t reported_offset,
                                           uint8_t data_length,
                                           const uint8_t* data,
                                           void* arg );

typedef void ( *ZNET_BACKUP_WRITE_RESULT )( znet_error_t err,
                                            uint16_t requested_offset,
                                            znet_backup_status_t status,
                                            uint16_t reported_offset,
                                            uint8_t data_length,
                                            void* arg );

typedef struct znet_cmd_anti_theft_report_t {
    bool is_locked;
    bool is_restricted;
    uint8_t hint_length;
    const uint8_t* hint;
    uint16_t manufacturer_id;
    uint16_t zwave_locking_id;
} znet_cmd_anti_theft_report_t;

typedef znet_cmd_anti_theft_report_t znet_cmd_anti_theft_unlock_state_report_t;

typedef void ( *ZNET_ANTI_THEFT_RESULT )( znet_error_t err,
                                          znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          const znet_cmd_anti_theft_report_t* value,
                                          void* arg );

typedef void ( *ZNET_ANTI_THEFT_UNLOCK_STATE_RESULT )( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       const znet_cmd_anti_theft_unlock_state_report_t* value,
                                                       void* arg );

typedef bool ( *ZNET_GET_KNOWN_S0_KEY )( uint8_t* buf, void* arg );

/**
 * @brief TBD.
 */
typedef struct znet_callbacks_t {
    void* arg; /**< Parameter for callback functions */

    ZNET_ALLOC alloc; /**< Func for memmory management [req] */
    ZNET_CLOCK clock; /**< Func for get monotonic time [req] */
    ZNET_LOG log;     /**< Func for logging [req] */

    ZNET_UART_WRITE uart_write; /**< Func for write data to uart [req] */
    ZNET_UART_READ uart_read;   /**< Func for read data from uart [req] */
    ZNET_UART_RESET uart_reset; /**< Func for reset uart (or buffers) [opt] */

    ZNET_STORE_SAVE store_save;   /**< Func for save data [req] */
    ZNET_STORE_LOAD store_load;   /**< Func for load data [req] */
    ZNET_STORE_RESET store_reset; /**< Func for reset store [req] */

    ZNET_INIT_FINISH
    init_finish; /**< Func for async result of init */

    ZNET_SET_DEFAULT_START
    set_default_started; /**< Func for async notify of set_default start [opt] */

    ZNET_SET_DEFAULT_RESULT
    set_default; /**< Func for async result of set_default [opt] */

    ZNET_SET_DEFAULT_RESULT
    set_default_raw_result; /**< Func for async notify of set_default start [opt] */

    ZNET_NETWORK_UPDATE_RESULT
    network_update; /**< Func for async result of network_update [opt] */
    ZNET_REQUEST_NEIGHBOR_RESULT
    request_neighbor_result; /**< Func for async result of request_neighbor [opt] */

    ZNET_PRIVATE_S2_DSK_RESULT
    private_s2_dsk;

    ZNET_NODE_ADD_RESULT
    node_add_result; /**< Func for async result of node_add [opt] */
    ZNET_NODE_ADD_SECURITY_FAILED
    node_add_security_failed; /**< Func to notify that a node has refused the inclusion [opt] */
    ZNET_NODE_ADD_INVOKED
    node_add_invoked; /**< Callback about include start */
    ZNET_NODE_ADD_RESULT
    node_add_break_result; /**< Func for async result of node_add [opt] */
    ZNET_NODE_REM_RESULT
    node_rem_result; /**< Func for async result of node_rem [opt] */
    ZNET_NODE_REM_RESULT
    node_rem_break_result; /**< Func for async result of node_rem [opt] */
    ZNET_NODE_REM_INVOKED
    node_rem_invoked; /**< Callback about include start */
    ZNET_NODE_LEARN_INVOKED
    node_learn_invoked; /**< Callback about learn start */
    ZNET_NODE_REPLACE_IF_FAILED_RESULT
    node_replace_if_failed_result; /**< Func for async result of node_replace_if_failed [opt] */
    ZNET_NODE_LEARN_RESULT
    node_learn_result; /**< Func for async result of node_learn [opt] */
    ZNET_NODE_LEARN_RESULT
    node_learn_break_result; /**< Func for async result of node_learn [opt] */
    ZNET_NODE_LIST_RESULT
    node_list_result; /**< Func for async result of node_list [opt] */
    ZNET_NODE_UPDATE_INFO_RESULT
    node_update_info_result; /**< Func for async result of node_request_info or
                                notifications [opt] */

    ZNET_NODE_S2_SELECT_KEYS
    node_s2_select_keys; /**< Func for request keys during S2 inclusion [opt] */
    ZNET_NODE_S2_REQUEST_PUB_KEY
    node_s2_request_pub_key; /**< Func for request keys during S2 inclusion
                                [opt] */
    ZNET_NODE_S2_SHOW_PUB_KEY
    node_s2_show_pub_key; /**< Func for request keys during S2 inclusion [opt]*/

    ZNET_NODE_INIF_RECEIVED
    node_inif_received; /**< Informs that a Smart Start node has been powered up */
    ZNET_NODE_NHID_RECEIVED
    node_nhid_received; /**< Informs that a Smart Start node has requested inclusion */

    RESET_ZWAVE_CHIP
    reset_zwave_chip;

    ZNET_SIS_UPDATE
    sis_updated; /**< Func for sis_update [opt] */

    ZNET_LIBRARY_INFO
    library_info; /**/

    ZNET_FORCED_REQUEST_RESULT
    forced_request_result; /**< Func for forced request timeout [opt] */

    ZNET_CHECK_NODE_SUPPORTS_COMMAND_CLASS
    check_node_supports_command_class;

    ZNET_NODE_CMD_VERSION_RESULT
    node_cmd_version_result; /**< Function for async result/report of
                                cmd_version [opt] */
    ZNET_NODE_CMD_COMMAND_VERSION_RESULT
    node_cmd_command_version_result; /**< Function for async result/report of
                                        cmd_command_version [opt] */
    ZNET_NODE_CMD_VERSION_CAPABILITIES_RESULT
    node_cmd_version_capabilities_result;

    ZNET_NODE_CMD_SOFTWARE_VERSION_RESULT
    node_cmd_software_version_result;

    ZNET_NODE_CMD_VERSION_FILL
    node_cmd_version_fill; /**< Function for request of
                                       cmd_version_fill [opt] */
    ZNET_NODE_CMD_MANUFACTURER_SPECIFIC_RESULT
    node_cmd_manufacturer_specific_result; /**< Function for async result/report
                                              of cmd_manufacturer_specific [opt] */
    ZNET_NODE_CMD_MANUFACTURER_SPECIFIC_FILL
    node_cmd_manufacturer_specific_fill; /**< Function for request of
                                          cmd_manufacturer_specific_fill [opt]*/
    ZNET_NODE_CMD_DEVICE_SPECIFIC_RESULT
    node_cmd_device_specific_result; /**< Function for async result/report of
                                              cmd_device_specific [opt] */
    ZNET_NODE_CMD_ZWAVEPLUS_INFO_RESULT
    node_cmd_zwaveplus_info_result; /**< Function for async result/report of
                                       cmd_zwaveplus_info [opt] */
    ZNET_NODE_CMD_ZWAVEPLUS_INFO_FILL
    node_cmd_zwaveplus_info_fill; /**< Function for request of
                                       cmd_zwaveplus_info [opt] */

    ZNET_NODE_CMD_FIRMWARE_UPDATE_INFO_FILL
    node_cmd_firmware_update_info_fill; /**< Function for request of
                                             cmd_firmware_update_info [opt] */

    ZNET_NODE_CMD_BASIC_RESULT
    node_cmd_basic_result; /**< Func for async result/report of cmd_basic [opt]
                            */

    ZNET_NODE_CMD_BASIC_RESULT
    node_cmd_basic_set_result; /**< Func for async result/report of cmd_basic [opt]
                                */
    ZNET_NODE_CMD_BATTERY_LEVEL_RESULT
    node_cmd_battery_level_result; /**< Func for async result/report of cmd_battery [opt]
                                    */
    ZNET_NODE_CMD_BINARY_SWITCH_RESULT
    node_cmd_binary_switch_result; /**< Func for async result/report of
                                      cmd_binary_switch [opt] */

    ZNET_NODE_CMD_CLOCK_RESULT
    node_cmd_clock_result; /**< Func for async result/report of
                                      cmd_clock [opt] */

    ZNET_NODE_CMD_TIME_PARAMETERS_RESULT
    node_cmd_time_parameters_result; /**< Func for async result/report of
                              cmd_time_parameters [opt] */

    ZNET_NODE_CMD_DOOR_LOCK_OPERATION_RESULT
    node_cmd_door_lock_operation_result; /**< Func for async result/report
                                          of cmd_door_lock_operation [opt] */

    ZNET_NODE_CMD_DOOR_LOCK_CONFIGURATION_RESULT
    node_cmd_door_lock_configuration_result; /**< Func for async result/report
                                            of cmd_door_lock_configuration [opt] */

    ZNET_NODE_CMD_DOOR_LOCK_CAPABILITIES_RESULT
    node_cmd_door_lock_capabilities_result; /**< Func for async result/report
                                            of cmd_door_lock_capabilities [opt] */

    ZNET_NODE_CMD_USER_CODE_REPORT
    node_cmd_user_code_report; /**< Func for async result/report
                               of cmd_user_code [opt] */

    ZNET_NODE_CMD_USERS_NUMBER_REPORT
    node_cmd_users_number_report; /**< Func for async result/report
                                  of cmd_users_number [opt] */

    ZNET_NODE_CMD_ENTRY_CONTROL_NOTIFICATION
    node_cmd_entry_control_notification; /**< Func for async result/report
                                                  of cmd_entry_control_notification [opt] */

    ZNET_NODE_CMD_ENTRY_CONTROL_KEY_SUPPORTED_REPORT
    node_cmd_entry_control_key_supported_report; /**< Func for async result/report
                                                  of cmd_entry_control_key_supported [opt] */

    ZNET_NODE_CMD_ENTRY_CONTROL_EVENT_SUPPORTED_REPORT
    node_cmd_entry_control_event_supported_report; /**< Func for async result/report
                                                  of cmd_entry_control_event_supported [opt] */

    ZNET_NODE_CMD_ENTRY_CONTROL_CONFIGURATION_REPORT
    node_cmd_entry_control_configuration_report; /**< Func for async result/report
                                                  of cmd_entry_control_configuration [opt] */

    ZNET_NODE_CMD_NOTIFICATION_REPORT
    node_cmd_notification_result; /**< Func for async result/report
                                  of cmd_notification [opt] */

    ZNET_NODE_CMD_NOTIFICATION_SUPPORTED_REPORT
    node_cmd_notification_supported_report; /**< Func for async result/report
                                  of cmd_notification_supported [opt] */

    ZNET_NODE_CMD_NOTIFICATION_EVENT_SUPPORTED_REPORT
    node_cmd_notification_event_supported_report; /**< Func for async result/report
                                  of cmd_notification_event_supported [opt] */
    ZNET_NODE_CMD_HAIL_REPORT
    node_cmd_hail_result; /**< Func for async result/report
                                  of cmd_hail [opt] */
    ZNET_NODE_CMD_METER_RESULT
    node_cmd_meter_result; /**< Func for async result/report of cmd_meter [opt]
                            */
    ZNET_NODE_CMD_METER_SET_DONE
    node_cmd_meter_set_done; /**< Func for async result/report of cmd_meter_reset [opt]
                              */
    ZNET_NODE_CMD_METER_SUPPORTED_RESULT
    node_cmd_meter_supported_result; /**< Func for async result/report of
                                        cmd_meter_supported [opt] */

    ZNET_NODE_CMD_MULTILEVEL_SENSOR_GET_SUPPORTED_SENSOR_RESULT
    node_cmd_multilevel_sensor_get_supported_sensor_result; /**< Func for async result/report of
                                                               cmd_multilevel_sensor_get_supported_sensor [opt]*/

    ZNET_NODE_CMD_MULTILEVEL_SENSOR_GET_SUPPORTED_SCALE_RESULT
    node_cmd_multilevel_sensor_get_supported_scale_result; /**< Func for async result/report of
                                                              cmd_multilevel_sensor_get_supported_scale [opt]*/

    ZNET_NODE_CMD_MULTILEVEL_SENSOR_GET_RESULT
    node_cmd_multilevel_sensor_get_result; /**< Func for async result/report of cmd_multilevel_sensor_get [opt]*/

    ZNET_NODE_CMD_MULTILEVEL_SWITCH_RESULT
    node_cmd_multilevel_switch_result; /**< Func for async result/report of
                                      cmd_multilevel_switch [opt] */

    ZNET_NODE_CMD_MULTILEVEL_SWITCH_SET_DONE
    node_cmd_multilevel_switch_set_done; /**< Func for async result/report of cmd_multilevel_switch_set [opt]
                                          */

    ZNET_NODE_CMD_MULTILEVEL_SWITCH_SUPPORTED_REPORT
    node_cmd_multilevel_switch_supported_result; /**< Func for async result/report
                                                  of cmd_multilevel_switch_supported [opt] */

    ZNET_NODE_CMD_BARRIER_OPERATOR_RESULT
    node_cmd_barrier_operator_result; /**< Func for async result/report of
                                      cmd_barrier_operator [opt] */

    ZNET_NODE_CMD_BARRIER_OPERATOR_SIGNAL_RESULT
    node_cmd_barrier_operator_signal_result; /**< Func for async result/report of
                                      cmd_barrier_operator [opt] */

    ZNET_NODE_CMD_BARRIER_OPERATOR_SET_DONE
    node_cmd_barrier_operator_set_done; /**< Func for async result/report of cmd_barrier_operator_set [opt]
                                         */

    ZNET_NODE_CMD_BARRIER_OPERATOR_SUPPORTED_SIGNALS_RESULT
    node_cmd_barrier_operator_supported_signals_result; /**< Func for async result/report of
                                      cmd_barrier_operator [opt] */

    ZNET_NODE_CMD_MULTICHANNEL_ENDPOINT_RESULT
    node_cmd_multichannel_endpoint_result; /**< Func for async result/report of
                                                cmd_multichannel_endpoint [opt]
                                            */
    ZNET_NODE_CMD_MULTICHANNEL_CAPABILITY_RESULT
    node_cmd_multichannel_capability_result; /**< Func for async result/report
                                                of cmd_multichannel_capability
                                                [opt] */
    ZNET_NODE_CMD_MULTICHANNEL_ENDPOINT_FIND_RESULT
    node_cmd_multichannel_endpoint_find_result; /**< Func for async
                                                result/report of
                                                cmd_multichannel_endpoint_find
                                                [opt] */
    ZNET_NODE_CMD_MULTICHANNEL_AGGREGATED_MEMBERS_RESULT
    node_cmd_multichannel_aggregated_members_result; /**< Func for async
                                                result/report of
                                                cmd_multichannel_aggregated_members
                                                [opt] */
    ZNET_NODE_CMD_CONFIGURATION_RESULT
    node_cmd_configuration_result; /**< Func for async result/report of
                                      cmd_configiration [opt] */
    ZNET_NODE_CMD_CONFIGURATION_REQUEST_STATUS
    node_cmd_configuration_request_status; /**< Func for async result of
                                              cmd_configiration [opt] */
    ZNET_NODE_CMD_CONFIGURATION_BULK_RESULT
    node_cmd_configuration_bulk_result; /**< Func for async result/report of
                                      cmd_configiration_bulk [opt] */
    ZNET_NODE_CMD_CONFIGURATION_NAME_RESULT
    node_cmd_configuration_name_result; /**< Func for async result/report of
                                      cmd_configiration_name [opt] */
    ZNET_NODE_CMD_CONFIGURATION_INFO_RESULT
    node_cmd_configuration_info_result; /**< Func for async result/report of
                                      cmd_configiration_info [opt] */
    ZNET_NODE_CMD_CONFIGURATION_PROPERTIES_RESULT
    node_cmd_configuration_properties_result; /**< Func for async result/report
                                      of cmd_configiration_properties [opt] */

    ZNET_NODE_CMD_ALARM_SENSOR_GET_RESULT
    node_cmd_alarm_sensor_get_result; /**< Func for async result/report
                                              of sensor_alarm_get [opt] */

    ZNET_NODE_CMD_ALARM_SENSOR_GET_SUPPORTED_SENSOR_RESULT
    node_cmd_alarm_sensor_get_supported_sensor_result; /**< Func for async result/report
                                          of sensor_alarm_supported_get [opt] */

    ZNET_NODE_CMD_BINARY_SENSOR_GET_RESULT
    node_cmd_binary_sensor_get_result; /**< Func for async result/report
                                          of cmd_configiration_properties [opt] */

    ZNET_NODE_CMD_BINARY_SENSOR_GET_SUPPORTED_SENSOR_RESULT
    node_cmd_binary_sensor_get_supported_sensor_result; /**< Func for async result/report
                                      of cmd_configiration_properties [opt] */

    ZNET_NODE_CMD_COLOR_SWITCH_RESULT
    node_cmd_color_switch_result; /**< Func for async result/report
                                       of cmd_color_switch_get [opt]>*/
    ZNET_NODE_CMD_COLOR_SWITCH_SUPPORTED_REPORT
    node_cmd_color_switch_supported_result; /** Func for async result/report
                                       of cmd_color_switch_supported_get */
    ZNET_NODE_CMD_POWERLEVEL_RESULT
    node_cmd_powerlevel_result; /**< Func for async result/report of
                                   cmd_powerlevel [opt] */
    ZNET_NODE_CMD_POWERLEVEL_TEST_NODE_RESULT
    node_cmd_powerlevel_test_node_result; /**< Func for async result/report of
                                             cmd_powerlevel_test_node [opt] */

    ZNET_NODE_CMD_SCENE_ACTUATOR_CONFIGURATION_RESULT
    node_cmd_scene_actuator_configuration_result; /**< Func for async result/report of
                                             cmd_scene_controller_actuator [opt] */
    ZNET_NODE_CMD_SCENE_CONTROLLER_CONFIGURATION_RESULT
    node_cmd_scene_controller_configuration_result; /**< Func for async result/report of
                                             cmd_scene_controller_configuration [opt] */
    ZNET_NODE_CMD_CENTRAL_SCENE_CONFIGURATION_RESULT
    node_cmd_central_scene_configuration_result; /**< Func for async result/report of
                                             cmd_central_scene_configuration [opt] */
    ZNET_NODE_CMD_CENTRAL_SCENE_SUPPORTED_RESULT
    node_cmd_central_scene_supported_result; /**< Func for async result/report of
                                             cmd_central_scene_supported [opt] */
    ZNET_NODE_CMD_CENTRAL_SCENE_NOTIFICATION
    node_cmd_central_scene_notification; /**< Function for notification of
                                         cmd_central_scene [opt] */

    ZNET_NODE_CMD_SCENE_ACTIVATION_SET_RESULT
    node_cmd_scene_activation_set_result;

    ZNET_NODE_DEVICE_RESET_LOCALLY
    node_cmd_device_reset_localy_result; /**< Func for async notification of
                                   device_reset_local [opt] */

    ZNET_NODE_CMD_INCLUSION_CONTROLLER_INITIATE
    node_cmd_inclusion_controller_initiate; /**< Func for inclusion controller request [opt] */

    ZNET_NODE_CMD_INCLUSION_CONTROLLER_COMPLETE
    node_cmd_inclusion_controller_complete; /**< Func for inclusion controller request [opt] */

    ZNET_NODE_CMD_NODE_PROVISIONING_SET
    node_cmd_node_provisioning_set; /**< Func for node provisioning set [opt] */

    ZNET_NODE_CMD_NODE_PROVISIONING_DELETE
    node_cmd_node_provisioning_delete; /**< Func for node provisioning delete [opt] */

    ZNET_NODE_CMD_NODE_PROVISIONING_GET
    node_cmd_node_provisioning_get; /**< Func for node provisioning get [opt] */

    ZNET_NODE_CMD_NODE_PROVISIONING_LIST_ITERATION_GET
    node_cmd_node_provisioning_list_iteration_get; /**< Func for node provisioning list iteration get [opt] */

    ZNET_NODE_CMD_SECURITY_SUPPORTED_RESULT
    node_cmd_security_supported_result; /**< Func for async result/report of
                                           cmd_security_supported
                                           [opt] */
    ZNET_NODE_CMD_ASSOCIATION_RESULT
    node_cmd_association_result; /**< Func for async result/report of
                                    cmd_association [opt] */
    ZNET_NODE_CMD_ASSOCIATION_SET_DONE
    node_cmd_association_set_done;

    ZNET_NODE_CMD_ASSOCIATION_REMOVE_DONE
    node_cmd_association_remove_done;

    ZNET_NODE_CMD_ASSOCIATION_SUPPORTED_GROUPING_RESULT
    node_cmd_association_supported_grouping_result; /**< Func for async
                                                       result/report of
                                                       cmd_association_supported_grouping
                                                       [opt] */
    ZNET_NODE_CMD_ASSOCIATION_SPECIFIC_GROUP_RESULT
    node_cmd_association_specific_group_result; /**< Func for async
                                                   result/report of
                                                   cmd_association_specific_group
                                                   [opt] */
    ZNET_NODE_CMD_ASSOCIATION_GROUP_NAME_RESULT
    node_cmd_association_group_name_result; /**< Func for async result/report of
                                               cmd_association_group_name [opt]
                                             */
    ZNET_NODE_CMD_ASSOCIATION_GROUP_INFO_RESULT
    node_cmd_association_group_info_result; /**< Func for async result/report of
                                               cmd_association_group_info [opt]
                                             */
    ZNET_NODE_CMD_ASSOCIATION_GROUP_COMMAND_LIST_RESULT
    node_cmd_association_group_command_list_result; /**< Func for async
                                                       result/report of
                                                       cmd_association_group_command_list
                                                       [opt] */
    ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_RESULT
    node_cmd_multichannel_association_result; /**< Func for async result/report
                                                 of cmd_multichannel_association
                                                 [opt] */
    ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_SET_DONE
    node_cmd_multichannel_association_set_done;

    ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_REMOVE_DONE
    node_cmd_multichannel_association_remove_done;

    ZNET_NODE_CMD_MULTICHANNEL_ASSOCIATION_SUPPORTED_GROUPING_RESULT
    node_cmd_multichannel_association_supported_grouping_result; /**< Func for
                                                                    async
                                                                    result/report
                                                                    of
                                                                    cmd_multichannel_association_supported_grouping
                                                                    [opt] */

    ZNET_NODE_CMD_SIMPLE_AV_CONTROL_SET_RESULT
    node_cmd_simple_av_control_set_result;

    ZNET_NO_OPERATION_RESULT
    node_cmd_no_operation_result; /**< Func for check/update reachable node
                                     state [opt] */

    ZNET_NODE_CMD_WAKEUP_RESULT
    node_cmd_wakeup_result; /**< Function for async result/report of
                                cmd_wakeup [opt] */

    ZNET_NODE_CMD_WAKEUP_NOTIFICATION
    node_cmd_wakeup_notification; /**< Function for notification of
                                cmd_wakeup [opt] */

    ZNET_NODE_CMD_WAKEUP_CAPABILITIES_RESULT
    node_cmd_wakeup_capabilities_result; /**< Function for async result/report
                                of cmd_wakeup [opt] */

    ZNET_NODE_CMD_THERMOSTAT_MODE_RESULT
    node_cmd_thermostat_mode_result;

    ZNET_NODE_CMD_THERMOSTAT_MODE_SUPPORTED_RESULT
    node_cmd_thermostat_mode_supported_result;

    ZNET_NODE_CMD_THERMOSTAT_OPERATING_STATE_RESULT
    node_cmd_thermostat_operating_state_result;

    ZNET_NODE_CMD_THERMOSTAT_OPERATING_STATE_LOGGING_SUPPORTED_RESULT
    node_cmd_thermostat_operating_state_logging_supported_result;

    ZNET_NODE_CMD_THERMOSTAT_OPERATING_STATE_LOGGING_RESULT
    node_cmd_thermostat_operating_state_logging_result;

    ZNET_NODE_CMD_THERMOSTAT_FAN_MODE_RESULT
    node_cmd_thermostat_fan_mode_result;

    ZNET_NODE_CMD_THERMOSTAT_FAN_MODE_SUPPORTED_RESULT
    node_cmd_thermostat_fan_mode_supported_result;

    ZNET_NODE_CMD_THERMOSTAT_FAN_STATE_RESULT
    node_cmd_thermostat_fan_state_result;

    ZNET_NODE_CMD_THERMOSTAT_SETBACK_RESULT
    node_cmd_thermostat_setback_result;

    ZNET_NODE_CMD_THERMOSTAT_SETPOINT_RESULT
    node_cmd_thermostat_setpoint_result;

    ZNET_NODE_CMD_THERMOSTAT_SETPOINT_SUPPORTED_RESULT
    node_cmd_thermostat_setpoint_supported_result;

    ZNET_NODE_CMD_THERMOSTAT_SETPOINT_CAPABILITIES_RESULT
    node_cmd_thermostat_setpoint_capabilities_result;

    ZNET_NODE_ZWAVE_PLUS_INFO_GET
    node_cmd_zwave_plus_info_get;

    ZNET_INDICATOR_IDENTIFY
    node_cmd_indicator_identify;

    ZNET_INDICATOR_RESULT
    node_cmd_indicator_result;

    ZNET_INDICATOR_SUPPORTED_RESULT
    node_cmd_indicator_supported_result;

    ZNET_NODE_IS_ADDED
    node_cmd_is_added;

    ZNET_NODE_SECURITY_GET
    node_cmd_security_get;

    ZNET_NODE_SECURITY_SET
    node_cmd_security_set;

    ZNET_NODE_IS_SUPPORTED_CC
    node_cmd_is_supported_cc;

    ZNET_NODE_IS_WAKEUP
    node_cmd_is_wakeup;

    ZNET_NODE_TIME_REQUESTED
    node_cmd_time_requested;

    ZNET_NODE_DATE_REQUESTED
    node_cmd_date_requested;

    ZNET_NODE_TIME_ZONE_REQUESTED
    node_cmd_time_zone_requested;

    ZNET_NODE_TIME_ZONE_SET
    node_cmd_time_zone_set;

    ZNET_NODE_TIME_ZONE_RESULT
    node_cmd_time_zone_result;

    ZNET_NODE_LOCAL_DATE_RESULT
    node_cmd_local_date_result;

    ZNET_NODE_LOCAL_TIME_RESULT
    node_cmd_local_time_result;

    ZNET_NODE_SCHEDULE_LOCK_ENTRY_TIME_OFFSET_RESULT
    node_schedule_lock_entry_time_offset_result;

    ZNET_NODE_SCHEDULE_LOCK_ENTRY_WEEK_DAY_RESULT
    node_schedule_lock_entry_week_day_result;

    ZNET_NODE_SCHEDULE_LOCK_ENTRY_YEAR_DAY_RESULT
    node_schedule_lock_entry_year_day_result;

    ZNET_NODE_SCHEDULE_LOCK_ENTRY_SUPPORTED_RESULT
    node_schedule_lock_entry_supported_result;

    ZNET_NODE_SCHEDULE_LOCK_ENTRY_DAILY_REPEATING_RESULT
    node_schedule_lock_entry_daily_repeating_result;

    ZNET_NODE_CMD_SOUND_SWITCH_TONES_NUMBER_RESULT
    node_cmd_sound_switch_tones_number_result;

    ZNET_NODE_CMD_SOUND_SWITCH_TONE_INFO_RESULT
    node_cmd_sound_switch_tone_info_result;

    ZNET_NODE_CMD_SOUND_SWITCH_CONFIGURATION_RESULT
    node_cmd_sound_switch_configuration_result;

    ZNET_NODE_CMD_SOUND_SWITCH_TONE_PLAY_RESULT
    node_cmd_sound_switch_tone_play_result;

    ZNET_NODE_CMD_USER_CODE_CAPABILITIES_REPORT
    node_cmd_user_code_capabilities_result;

    ZNET_NODE_CMD_USER_CODES_KEYPAD_MODE_REPORT
    node_cmd_user_codes_keypad_mode_result;

    ZNET_NODE_CMD_EXTENDED_USER_CODE_REPORT
    node_cmd_extended_user_code_result;

    ZNET_NODE_CMD_USER_CODE_CHECKSUM_REPORT
    node_cmd_user_code_checksum_result;

    ZNET_NODE_CMD_MASTER_CODE_REPORT
    node_cmd_master_code_result;

    ZNET_NODE_FIRMWARE_MD_RESULT node_cmd_firmware_md_result;

    ZNET_NODE_FIRMWARE_UPDATE_MD_REQUEST_RESULT node_cmd_firmware_update_md_request_result;

    ZNET_NODE_FIRMWARE_UPDATE_MD_REQUESTED node_cmd_firmware_update_md_requested;

    ZNET_NODE_FIRMWARE_UPDATE_MD_RESULT node_cmd_firmware_update_md_result;

    ZNET_NODE_FIRMWARE_UPDATE_MD_STATUS_RESULT node_cmd_firmware_update_md_status_result;

    ZNET_NODE_FIRMWARE_UPDATE_MD_ACTIVATION_STATUS_RESULT node_cmd_firmware_update_md_activation_status_result;

    ZNET_NODE_FIRMWARE_UPDATE_MD_PREPARATION_RESULT node_cmd_firmware_update_md_preparation_result;

    ZNET_BACKUP_SESSION_OPEN_RESULT backup_session_open_result;

    ZNET_BACKUP_SESSION_CLOSE_RESULT backup_session_close_result;

    ZNET_BACKUP_READ_RESULT backup_read_result;

    ZNET_BACKUP_WRITE_RESULT backup_write_result;

    ZNET_ANTI_THEFT_RESULT node_cmd_anti_theft_result;

    ZNET_ANTI_THEFT_UNLOCK_STATE_RESULT node_cmd_anti_theft_unlock_state_result;
    /// TODO: to declare others callback functions

    ZNET_GET_KNOWN_S0_KEY get_known_S0_key;
} znet_callbacks_t;

/**
 * @brief Init znet library
 *
 * @param Callbacks. Do not delete the znet_callbacks_t structure while using
 * the library!!!
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 */
znet_error_t znet_init( const znet_callbacks_t* callbacks );

void znet_deinit( void );

/**
 * @brief Main handler
 *
 * Need periodically call to process incoming data from the chip.
 *
 * Example of use case:
 * @code
 * for(;;) {
 *     znet_proc();
 *     usleep(1000);
 * }
 * @endcode
 */
void znet_proc( void );

/**
 * @brief Send DEVICE_RESET_LOCALLY notifications
 *
 * Send DEVICE_RESET_LOCALLY_NOTIFICATION to all nodes in the Lifeline association group.
 *
 * @param on_finish Callback to invoke when all notifications have been sent
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
void znet_send_device_reset_locally_notifications( void ( *on_finish )( void ) );

/**
 * @brief Set default
 *
 * Set the Controller back to the factory default state.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_set_default( void );

znet_error_t znet_set_default_raw( void );

znet_error_t znet_serial_api_soft_reset( void );

/**
 * @brief Send NIF
 *
 * Create and transmit a "Node Information" frame.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_send_node_info( void );

/**
 * @brief Set the SUC node's Id.
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_assign_suc_return_route( znet_node_id_t node_id );

/**
 * @brief Network update
 *
 * Request a network update from a SUC/SIS controller.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_network_update( void );

/**
 * @brief Request neighbor
 *
 * Request a node neighbor update
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_request_node_neighbor( znet_node_id_t node_id );

/**
 * @brief Request background RSSI
 *
 * Request background RSSI
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_get_background_RSSI();

/**
 * @brief Enter Smart Start listen mode to start receiving NHID/INIF frames from Smart Start nodes.
 */
znet_error_t znet_smart_start_listen_mode_enable();

/**
 * @brief Deactivate Smart Start listen mode.
 */
znet_error_t znet_smart_start_listen_mode_disable();

/**
 * @brief Add node to network
 *
 * Run the process of inclusion a new node to the network.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_add( void );

/**
 * @brief Initiate inclusion of a Smart Start node with the given DSK derived HomeID.
 *
 * @param dsk_derived_home_id Buffer containing 8-byte long DSK derived HomeID.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_add_dsk_to_network( const uint8_t* dsk_derived_home_id );

/**
 * @brief Stop add any node to network
 *
 * Break the process of inclusion a new node to the network.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_add_break( void );

/**
 * @brief Remove any node from network
 *
 * Run the process of exclusion a node from the enetwork.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_rem( void );

/**
 * @brief Remove failed node from network
 *
 * Run the process of exclusion a node from the enetwork.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_rem_failed_node( uint8_t node_id );

/**
 * @brief Stop remove any node from network
 *
 * Break the running process of exclusion a node from the enetwork.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_rem_break( void );

/**
 * @brief Replace failed node
 *
 * Run the process of replace failed node
 *
 * @param node_id Node ID for replace
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_replace_if_failed( znet_node_id_t node_id );

/**
 * @brief Start learn mode
 *
 * Used to instruct the controller to allow it to be added or removed from the
 * network.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_learn( void );

/**
 * @brief Stop learn mode (Ignored if in process of include or exclude)
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_learn_break( void );

/**
 * @brief Stop include device in secure mode
 */
void znet_node_sec2_abort_join( void );

/**
 * @brief Request nodes of SUC
 *
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_learn_nodes_request( void );

/**
 * @brief Update node info
 *
 * Run the process of update node info.
 *
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_request_info( znet_node_id_t node_id, uint8_t security );

/**
 * @brief Select S2 keys (response)
 *
 * @param selected_keys Selected keys (bitmask)
 */
void znet_node_s2_select_keys( znet_S2_keys_t selected_keys );

/**
 * @brief Enter S2 key (response)
 *
 * @param _key Selected keys (bitmask)
 */
void znet_node_s2_response_pub_key( const znet_S2_pub_key_t* pub_key );

/**
 * @brief Get version info
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_version_get( znet_node_id_t node_id );

/**
 * @brief Get command class version
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param command Command class
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_command_version_get( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_command_class_t command );

/**
 * @brief Get version capabilities
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_capabilities_version_get( znet_node_id_t node_id );

/**
 * @brief Get software version
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_software_version_get( znet_node_id_t node_id );

/**
 * @brief Get manufacturer specific information.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_manufacturer_specific_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Get device specific information.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param type Device ID Type
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_device_specific_get( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_cmd_device_specific_type_t type );

/**
 * @brief Get Z-Wave Plus Info.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_zwaveplus_info_get( znet_node_id_t node_id );

/**
 * @brief Operate primary functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_basic_set( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                      znet_cmd_basic_value_t value );

/**
 * @brief Operate primary functionality of node
 *
 * @param numberNodes quantity Node ID in List
 * @param pNodeIDList List of Node ID
 * @param channel_id Channel ID
 * @param value Value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_basic_set_multi( uint8_t nodeQuantity,
                                            const znet_node_id_t* listNodeId,
                                            znet_node_channel_id_t channel_id,
                                            znet_cmd_basic_value_t value,
                                            bool send_single_casts );

/**
 * @brief Get state for primary functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_basic_get( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );
/**
 * @brief Get state for primary functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_battery_level_get( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Operate binary switch functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_binary_switch_set( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                              znet_cmd_binary_switch_value_t value );

/**
 * @brief Operate binary switch functionality of node
 *
 * @param numberNodes quantity Node ID in List
 * @param pNodeIDList List of Node ID
 * @param channel_id Channel ID
 * @param value Value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_binary_switch_set_multi( uint8_t nodeQuantity,
                                                    const znet_node_id_t* listNodeId,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_binary_switch_value_t value,
                                                    bool send_single_casts );

/**
 * @brief Get state for binary switch functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_binary_switch_get( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Request the number of tones supported by the receiving node
 */
znet_error_t znet_node_cmd_sound_switch_tones_number_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Query the information associated to a tone at a supporting node
 */
znet_error_t znet_node_cmd_sound_switch_tone_info_get( uint8_t node_id, uint8_t channel_id, uint8_t tone_id );

/**
 * @brief Set the configuration for playing tones at the supporting node
 */
znet_error_t znet_node_cmd_sound_switch_configuration_set( uint8_t node_id,
                                                           uint8_t channel_id,
                                                           uint8_t volume,
                                                           uint8_t default_tone_id );

/**
 * @brief Request the current configuration for playing tones at the supporting node
 */
znet_error_t znet_node_cmd_sound_switch_configuration_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Instruct a supporting node to play (or stop playing) a tone
 */

znet_error_t znet_node_cmd_sound_switch_tone_play_set( uint8_t node_id,
                                                       uint8_t channel_id,
                                                       uint8_t tone_id,
                                                       uint8_t volume );

/**
 * @brief Request the current tone being played by the receiving node
 */
znet_error_t znet_node_cmd_sound_switch_tone_play_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Set clock value
 * @param node_id
 * @param channel_id
 * @param hour
 * @param minute
 * @param weekday
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_clock_set( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                      znet_clock_hour_value_t hour,
                                      znet_clock_minute_value_t minute,
                                      znet_clock_weekday_value_t weekday );

/**
 * @brief Get clock value
 * @param node_id
 * @param channel_id
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_clock_get( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Get data and time parameters
 * @param node_id
 * @param channel_id
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_time_parameter_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set current date and time in Universal Time (UTC)
 * @param node_id
 * @param channel_id
 * @param date_utc
 * @param time_utc
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_time_parameters_set( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                const znet_date_t* date_utc,
                                                const znet_time_t* time_utc );

/**
 * @brief Set time zone settings for node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param settings Time Zone Settings
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_time_zone_set( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          const znet_time_zone_settings_t* settings );

/**
 * @brief Report time zone settings to node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param settings Time Zone Settings
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_time_zone_respond( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              const znet_time_zone_settings_t* settings,
                                              const uint8_t encap );

/**
 * @brief Requests time zone settings from node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_time_zone_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Requests actual local time from node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_local_time_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Requests actual local date from node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_local_date_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Reports actual local time on corresponding get request
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param local_time Local Time
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_local_time_respond( znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               const znet_local_time_t* local_time,
                                               const uint8_t encap );

/**
 * @brief Reports actual local date on corresponding get request
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param local_date Local Date
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_local_date_respond( znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               const znet_local_date_t* local_date,
                                               const uint8_t encap );

/**
 * @brief Enable or disable all schedules for type Entry Lock
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param is_enabled
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_enable_all_set( znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              bool is_enabled );

/**
 * @brief Enable or disable schedules for a specified user code ID
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User Identifier is used to recognize the user identity
 * @param is_enabled
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_enable_set( znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          uint8_t user_id,
                                                          bool is_enabled );

/**
 * @brief Request time zone offset and daylight savings parameters
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_time_offset_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Set the current local TZO and DST offsets into an Entry Lock Device
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param time_offset
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_time_offset_set(
    uint8_t node_id,
    uint8_t channel_id,
    const znet_cmd_schedule_entry_lock_time_offset_t* time_offset );

/**
 * @brief Get a week day schedule slot for a identified user and specified schedule slot ID
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User Identifier
 * @param slot_id Slot Identifier
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_week_day_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t user_id,
                                                            uint8_t slot_id );

/**
 * @brief Set or erase a weekday schedule for a identified user
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param action Erase/Modify
 * @param schedule
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_week_day_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t action,
                                                            const znet_week_day_schedule_t* schedule );

bool is_week_day_schedule_valid( const znet_week_day_schedule_t* schedule );

/**
 * @brief Get a year/day schedule slot for an identified user
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User Identifier
 * @param slot_id Slot Identifier
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_year_day_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t user_id,
                                                            uint8_t slot_id );

/**
 * @brief Set or erases a schedule slot for a identified user
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param action Erase/Modify
 * @param schedule
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_year_day_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t action,
                                                            const znet_year_day_schedule_t* schedule );

bool is_year_day_schedule_valid( const znet_year_day_schedule_t* schedule );

/**
 * @brief Get the number of schedule slots each type of schedule the device supports for every user
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_type_supported_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Get a daily repeating schedule slot for a identified user and specified schedule slot ID
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User Identifier
 * @param slot_id Slot Identifier
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_daily_repeating_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t user_id,
                                                                   uint8_t slot_id );

/**
 * @brief Set or erase a daily repeating schedule for an identified user
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param action Erase/Modify
 * @param schedule
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cc_schedule_entry_lock_daily_repeating_set( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t action,
                                                                   const znet_daily_repeating_schedule_t* schedule );

bool is_daily_repeating_valid( const znet_daily_repeating_schedule_t* schedule );

/**
 * @brief Set the configuration of a supporting door lock device
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_door_lock_configuration_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
    const znet_door_lock_configuration_t* configuration,
    const znet_door_lock_extended_configuration_t* extended_configuration );

/**
 * @brief Request the configuration parameters of a door lock device
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_door_lock_configuration_get(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Set the operation mode of a supporting door lock device
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_door_lock_operation_set( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
                                                    znet_cmd_door_lock_operation_set_t value );

/**
 * @brief Request the status of a door lock device
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_door_lock_operation_get( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Request door lock capabilities ( v4 )
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_door_lock_capabilities_get( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Configure Event Type specific parameters
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_entry_control_configuration_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
    znet_entry_control_configuration_set_t* value );

/**
 * @brief Request the number of user codes supported
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_users_number_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set user code
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User ID
 * @param user_id_status User ID status
 * @param user_code_size User code size (min 4 bytes)
 * @param user_code User code (digits encoded with ASCII representation (from 0x30 to 0x39))
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_user_code_set( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t user_id,
                                          uint8_t user_id_status,
                                          uint8_t user_code_size,
                                          const uint8_t* user_code );

/**
 * @brief Get user code
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_user_code_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id, uint8_t user_id );

/**
 * @brief Get user code capabilities ( V2 )
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_user_code_capabilities_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Set keypad mode ( V2 ). See available modes in user code capabilities
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param keypad_mode Keypad mode
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_user_code_keypad_mode_set( uint8_t node_id, uint8_t channel_id, uint8_t keypad_mode );

/**
 * @brief Get current keypad mode ( V2 ). See available modes in user code capabilities
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_user_code_keypad_mode_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Get extended user codes ( V2 )
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_id User ID
 * @param should_report_more - set true for requesting list of codes. See capabilities is multiple report supported.
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_extended_user_code_get( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   znet_supported_users_t user_id,
                                                   bool should_report_more );

/**
 * @brief Get extended user codes ( V2 )
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param user_codes_count - count of user codes in passed list ( it's not recommended to pass more 10 ). See
 * capabilities is multiple set supported.
 * @param user_codes - list of user codes
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_extended_user_code_set( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   uint8_t user_codes_count,
                                                   const znet_user_code_report_t* user_codes );

/**
 * @brief Set master code ( V2 ). See capabilities is master code supported
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param master_code_length Length of code. It can be 0 for master code deactivation if this option is supported (see
 * capabilities)
 * @param master_code Master code
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_master_code_set( uint8_t node_id,
                                            uint8_t channel_id,
                                            uint8_t master_code_length,
                                            const uint8_t* master_code );

/**
 * @brief Get master code ( V2 )
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_master_code_get( uint8_t node_id, uint8_t channel_id );

/**
 * @brief Get checksum of user codes ( V2 ). See capabilities is checksum supported
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_user_code_checksum_get( uint8_t node_id, uint8_t channel_id );

znet_user_code_checksum_t calculate_user_codes_checksum( const znet_user_code_report_t* values,
                                                         znet_supported_users_t count );

/**
 * @brief Request if the unsolicited transmission of a specific Notification Type is enabled
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param v1_alarm_type V1 Alarm Type if receiving node supports V1, 0 otherwise
 * @param type
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_notification_get( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
                                             uint8_t v1_alarm_type,
                                             uint8_t type,
                                             uint8_t event,
                                             uint8_t version );

/**
 * @brief Enable or disable the unsolicited transmission of a specific Notification Type
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_notification_set( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
                                             uint8_t type,
                                             uint8_t status );

/**
 * @brief Request supported Notification Types
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_notification_supported_get( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id ); /* = ZNET_CHANNEL_ID_ROOT */

/**
 * @brief Request supported Notifications for specific Notification Type
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */

znet_error_t znet_node_cmd_notification_event_supported_get(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
    uint8_t type );

/**
 * @brief Query the value of a configuration parameter
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param config_param_num Parameter Number
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_get( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                              uint8_t config_param_num );

/**
 * @brief Set the value of a configuration parameter
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param config_param_num Parameter Number
 * @param config_size Size of the actual parameter
 * @param set_to_default Default flag
 * @param config_value Configuration Value (N bytes)
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_set( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                              uint8_t config_param_num,
                                              uint8_t config_size,
                                              int set_to_default,
                                              znet_cmd_configuration_value_t config_value );

/**
 * @brief Set the value of one or more configuration parameters.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param config_id Parameter Offset
 * @param config_count Number  of the configuration parameters
 * @param config_size Size of the actual parameters
 * @param need_report Default flag
 * @param set_to_default Handshake (get report) flag
 * @param config_value Configuration Value (N bytes)
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_bulk_set( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                   znet_cmd_configuration_id_t config_id,
                                                   uint8_t config_count,
                                                   uint8_t config_size,
                                                   int need_report,
                                                   int set_to_default,
                                                   const uint8_t* config_value );

/**
 * @brief Query the value of one or more configuration parameters
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param config_id Parameter Offset
 * @param config_count Number  of the configuration parameters
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_bulk_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                   znet_cmd_configuration_id_t config_id,
                                                   uint8_t config_count );

/**
 * @brief Request the name of a configuration parameter
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param param_number Requested configuration parameter
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_name_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                   znet_cmd_configuration_id_t param_number,
                                                   bool do_request );

/**
 * @brief Request usage information for a configuration parameter
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param param_number Requested configuration parameter
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_info_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                   znet_cmd_configuration_id_t param_number,
                                                   bool do_request );

/**
 * @brief Request the properties of a configuration parameter
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param param_number Requested configuration parameter
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_properties_get( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                         znet_cmd_configuration_id_t param_number );

/**
 * @brief Reset all configuration parameters to their default value
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_configuration_default_reset(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Node cmd Binary Sensor Get
 * @param node_id       Destination Node Id
 * @param channel_id Channel ID
 * @param sensor_type   Type of sensor this command originates from
 * @return status operation
 */
znet_error_t znet_node_cmd_sensor_alarm_get( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             uint8_t sensor_type );

/**
 * @brief Node cmd Binary Sensor Get Supported Sensor
 * @param node_id       Destination Node Id
 * @param channel_id Channel ID
 * @return status operation
 */
znet_error_t znet_node_cmd_sensor_alarm_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channelId );

/**
 * @brief Node cmd set silent sensor alarms
 * @param node_id               Destination Node Id
 * @param channel_id            Channel ID
 * @param mode                  Sensor sound disabling mode
 * @param delay                 Disabling delay in seconds
 * @param sensors_bitmask_size  Size of passed bitmask in bytes
 * @param sensors_bitmask       Bitmask of sensors to be disabled. Table of notification types for more information
 * @return status operation
 */
znet_error_t znet_node_cmd_alarm_silence_set( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              uint8_t mode,
                                              uint16_t delay,
                                              uint8_t sensors_bitmask_size,
                                              const uint8_t* sensors_bitmask );

/**
 * @brief Node cmd Binary Sensor Get
 * @param node_id       Destination Node Id
 * @param channel_id Channel ID
 * @param sensor_type   Type of sensor this command originates from
 * @return status operation
 */
znet_error_t znet_node_cmd_sensor_binary_get( znet_node_id_t node_id,
                                              znet_node_channel_id_t channelId,
                                              uint8_t sensor_type );

znet_error_t znet_node_cmd_sensor_binary_get_v1( znet_node_id_t node_id, znet_node_channel_id_t channelId );

/**
 * @brief Node cmd Binary Sensor Get Supported Sensor
 * @param node_id       Destination Node Id
 * @param channel_id Channel ID
 * @param version       CC version
 * @return status operation
 */
znet_error_t znet_node_cmd_sensor_binary_supported_get( znet_node_id_t node_id,
                                                        znet_node_channel_id_t channelId,
                                                        uint8_t version );

/**
 * @brief Gets current value for color component
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param component_id Requested color component ID
 * @return Return ZNETT_ERR_OK on success if the task is queued for execution. On error, other value is returned.
 */
znet_error_t znet_node_cmd_color_switch_get( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             uint8_t component_id );

/**
 * @brief Sets values for color components IDs
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param cl_components_count Number of components to set
 * @param components Array of color components
 * @return Return ZNETT_ERR_OK on success if the task is queued for execution. On error, other value is returned.
 */
znet_error_t znet_node_cmd_color_switch_set( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             uint8_t cl_components_count,
                                             const znet_cl_component_t* components,
                                             uint8_t version );

/**
 * @brief Sets values for color components IDs
 *
 * @param numberNodes quantity Node ID in List
 * @param pNodeIDList List of Node ID
 * @param channel_id Channel ID
 * @param cl_components_count Number of components to set
 * @param components Array of color components
 * @return Return ZNETT_ERR_OK on success if the task is queued for execution. On error, other value is returned.
 */
znet_error_t znet_node_cmd_color_switch_set_multi( uint8_t nodeQuantity,
                                                   const znet_node_id_t* listNodeId,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t cl_components_count,
                                                   const znet_cl_component_t* components,
                                                   bool send_single_casts );

/**
 * @brief Starts smooth change of value for component ID
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param up_down Make value higher if false, lower if true
 * @param ignore_start_state Flag to ignore start level
 * @param cl_component_id Color component ID to change
 * @param start_level Starting color value
 * @param duration Duration for change in ms
 * @return Return ZNETT_ERR_OK on success if the task is queued for execution. On error, other value is returned.
 */
znet_error_t znet_node_cmd_color_switch_start_level_change( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            bool up_down,
                                                            bool ignore_start_state,
                                                            uint8_t cl_component_id,
                                                            uint8_t start_level,
                                                            uint8_t duration,
                                                            uint8_t version );

/**
 * @brief Stops change for color component ID
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param cl_component_id Color component ID to stop change
 * @return Return ZNETT_ERR_OK on success if the task is queued for execution. On error, other value is returned.
 */
znet_error_t znet_node_cmd_color_switch_stop_level_change( znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           uint8_t cl_component_id );

/**
 * @brief Gets mask with supported color components by node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNETT_ERR_OK on success if the task is queued for execution. On error, other value is returned.
 */
znet_error_t znet_node_cmd_color_switch_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Operate multilevel switch functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multilevel_switch_set( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                  znet_cmd_multilevel_switch_value_t value,
                                                  uint8_t duration_available,
                                                  uint8_t duration );

/**
 * @brief Operate multilevel switch functionality of node
 *
 * @param numberNodes quantity Node ID in List
 * @param pNodeIDList List of Node ID
 * @param channel_id Channel ID
 * @param value Value
 * @param duration_available Duration available
 * @param duration Duration
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multilevel_switch_set_multi( uint8_t numberNodes,
                                                        const znet_node_id_t* listNodeId,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_cmd_multilevel_switch_value_t value,
                                                        uint8_t duration_available,
                                                        uint8_t duration,
                                                        bool send_single_casts );

/**
 * @brief Get state for multilevel switch functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multilevel_switch_get( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

typedef enum znet_cmd_multilevel_switch_change_direction_t {
    ZNET_CMD_MULTILEVEL_SWITCH_CHANGE_DIRECTION_INC = 0,
    ZNET_CMD_MULTILEVEL_SWITCH_CHANGE_DIRECTION_DEC = 1,
    ZNET_CMD_MULTILEVEL_SWITCH_NO_CHANGE = 3
} znet_cmd_multilevel_switch_change_direction_t;

/**
 * @brief Initiate a transition to a new level on multilevel switch
 *
 * For ignore start value -> set start_value to
 * ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param direction Direction of change (0 - increasing, 1 - decreasing)
 * @param start_value Value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multilevel_switch_start_change(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
    znet_cmd_multilevel_switch_change_direction_t direction,
    znet_cmd_multilevel_switch_value_t start_value,
    uint8_t duration,
    uint8_t version );

/**
 * @brief Terminate a transition to a new level on multilevel switch
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multilevel_switch_stop_change(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Configure settings for a given physical item on the device.
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multilevel_switch_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Get state for barrier operator functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_barrier_operator_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Get supported signals for barrier operator functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_barrier_operator_signal_supported_get( znet_node_id_t node_id,
                                                                  znet_node_channel_id_t channel_id );

/**
 * @brief Get signal state for barrier operator functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_barrier_operator_signal_get( znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_cmd_barrier_operator_signal_t signal_id );

/**
 * @brief Set signal state for barrier operator functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_barrier_operator_signal_set( znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_cmd_barrier_operator_signal_t signal_id,
                                                        znet_cmd_barrier_operator_signal_state_t signal_state );

/**
 * @brief Set state for barrier operator functionality of node
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value state value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_barrier_operator_set( znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 znet_cmd_barrier_operator_value_t value );

/**
 * @brief Set state for barrier operator functionality of node
 *
 * @param numberNodes quantity Node ID in List
 * @param pNodeIDList List of Node ID
 * @param channel_id Channel ID
 * @param value state value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_barrier_operator_set_multi( uint8_t nodeQuantity,
                                                       const znet_node_id_t* listNodeId,
                                                       znet_node_channel_id_t channel_id,
                                                       znet_cmd_basic_value_t value,
                                                       bool send_single_casts );

/**
 * @brief Request the accumulated consumption in physical units from a metering
 * device.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param scale Scale advertise the unit used
 * @param version CC version
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_meter_get( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                      uint8_t scale,
                                      znet_command_class_version_t version );

znet_error_t znet_node_cmd_meter_unknown_scale_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Request the supported scales in a sub meter.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_meter_supported_get( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Reset the accumulated consumption in physical units on a metering
 * device.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_meter_reset( znet_node_id_t node_id,
                                        znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

znet_error_t znet_node_cmd_meter_reset_v6( znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           uint8_t meter_type,
                                           uint8_t data_size,
                                           const uint8_t* data );

// TODO
znet_error_t znet_node_cmd_multilevel_sensor_get_supported_sensor( znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id );
znet_error_t znet_node_cmd_multilevel_sensor_get_supported_scale( znet_node_id_t node_id,
                                                                  znet_node_channel_id_t channel_id,
                                                                  uint8_t sensor_type );
znet_error_t znet_node_cmd_multilevel_sensor_get( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t sensor_type,
                                                  uint8_t scale,
                                                  uint8_t version );

znet_error_t znet_node_cmd_multilevel_sensor_unknown_type_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id );

/**
 * @brief Query the number of End Points implemented by the node.
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_endpoint_get( znet_node_id_t node_id );

/**
 * @brief Query the non-secure capabilities of an End Point.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_capability_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Search for a specific set of Generic and Specific Device Class.
 *
 * @param node_id Node ID
 * @param generic Generic Class
 * @param specific Specific Class
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_endpoint_find( znet_node_id_t node_id,
                                                       znet_generic_class_t generic,
                                                       znet_specific_class_t specific,
                                                       bool do_request );

/**
 * @brief Query the members of an Aggregated End Point.
 *
 * @param node_id Node ID
 * @param channel_id Aggregated Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_aggregated_members_get( znet_node_id_t node_id,
                                                                znet_node_channel_id_t channel_id );

/**
 * @brief Request the current power level value.
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_powerlevel_get( znet_node_id_t node_id );

/**
 * @brief Set the power level indicator value.
 *
 * @param node_id Node ID
 * @param power_level Power level
 * @param timeout Timeout
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_powerlevel_set( znet_node_id_t node_id,
                                           znet_powerlevel_value_t power_level,
                                           znet_powerlevel_timeout_t timeout );

/**
 * @brief Request the result of the latest Powerlevel Test.
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_powerlevel_test_node_get( znet_node_id_t node_id );

/**
 * @brief Set the power level indicator value.
 *
 * @param node_id Node ID
 * @param test_id Test node ID
 * @param power_level Test power level
 * @param test_count Test frame count
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_powerlevel_test_node_set( znet_node_id_t node_id,
                                                     znet_node_id_t test_id,
                                                     znet_powerlevel_value_t power_level,
                                                     uint16_t test_count );

/**
 * @brief Activate the setting associated to the scene ID dimming duration value.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param sceneId Scene ID
 * @param dimmingDuration Dimming Duration value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_scene_activation_set( znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
                                                 znet_cmd_scene_id_t scene_id,
                                                 znet_cmd_scene_dimming_duration_t dimming_duration );

/**
 * @brief Associate the specified scene ID to the defined settings.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param scene_id Scene ID
 * @param dimmingDuration Dimming Duration value
 * @param override Define the level source associated to the Scene ID
 * @param level Level value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_scene_actuator_configuration_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
    znet_cmd_scene_id_t scene_id,
    znet_cmd_scene_dimming_duration_t dimming_duration,
    uint8_t is_override,
    znet_cmd_scene_level_t level );
/**
 * @brief Query the settings for a given scene identifier or for the currently active scene settings
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param scene_id Scene ID
 */
znet_error_t znet_node_cmd_scene_actuator_configuration_get( znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             znet_cmd_scene_id_t scene_id );

/**
 * @brief Configure settings for a given physical item on the device.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param groupId Group ID
 * @param sceneId Scene ID
 * @param dimming_duration Dimming Duration value
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_scene_controller_configuration_set( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_cmd_group_id_t group_id,
                                                               znet_cmd_scene_id_t scene_id,
                                                               znet_cmd_scene_dimming_duration_t dimming_duration );

/**
 * @brief Query the settings for a given grouping identifier or the active settings.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param group_id Group ID (8 bits)
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_scene_controller_configuration_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_cmd_group_id_t group_id );

/**
 * @brief Configure settings for a given physical item on the device.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param slow_refresh This flag is used to configure the Slow Refresh capability
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_central_scene_configuration_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */,
    znet_cmd_central_scene_value_t slow_refresh );

/**
 * @brief Configure settings for a given physical item on the device.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_central_scene_configuration_get( znet_node_id_t node, znet_node_channel_id_t channelId );

/**
 * @brief Configure settings for a given physical item on the device.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_central_scene_supported_get( znet_node_id_t node, znet_node_channel_id_t channelId );

/**
 * @brief Query the commands supported by the device when using secure
 * communication (current security).
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_security_supported_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id /* = ZNET_CHANNEL_ID_ROOT */ );

/**
 * @brief Request the current destinations of a given association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_get( znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            znet_grouping_id_t grouping_id );

/**
 * @brief Add destinations to a given association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param value Values
 * @param value_count Values count
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_set( znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            znet_grouping_id_t grouping_id,
                                            const znet_node_id_t* value,
                                            size_t value_count );

/**
 * @brief Remove destinations from a given association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param value Values
 * @param value_count Values count
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_remove( znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               znet_grouping_id_t grouping_id,
                                               const znet_node_id_t* value,
                                               size_t value_count );

/**
 * @brief Request the number of association groups that this node supports.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_supported_grouping_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id );

/**
 * @brief Request the group that represents the most recently detected button.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_specific_group_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Query the name of an association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param grouping_id Grouping ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_group_name_get( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       znet_grouping_id_t grouping_id );

/**
 * @brief Request the properties of one or more association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param grouping_id Grouping ID
 * @param list_mode List mode
 * @param refresh_cache Refresh cache
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_group_info_get( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       znet_grouping_id_t grouping_id,
                                                       int list_mode,
                                                       int refresh_cache );

/**
 * @brief Request the commands that are sent via a given association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param grouping_id Grouping ID
 * @param allow_cache Allow cache
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_association_group_command_list_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_grouping_id_t grouping_id,
                                                               int allow_cache );

/**
 * @brief Request the current destinations of a given multi channel association
 * group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_association_get( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         znet_grouping_id_t grouping_id );

/**
 * @brief Add destinations to a given multi channel association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param value Values
 * @param value_count Values count
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_association_set( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         znet_grouping_id_t grouping_id,
                                                         const znet_endpoint_t* value,
                                                         uint8_t value_count );

/**
 * @brief Remove destinations from a given multi channel association group.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param grouping_id Grouping ID
 * @param value Values
 * @param value_count Values count
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_association_remove( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            znet_grouping_id_t grouping_id,
                                                            const znet_endpoint_t* value,
                                                            uint8_t value_count );

/**
 * @brief Request the number of multi channel association groups that this node
 * supports.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_multichannel_association_supported_grouping_get( znet_node_id_t node_id,
                                                                            znet_node_channel_id_t channel_id );

/**
 * @brief Send no operation request for checking current state of device
 *
 * If node is reachable, Zero result will be returned in
 * node_cmd_no_operation_result callback
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_no_operation( znet_node_id_t node_id );

/**
 * @brief Send Node Provisioning report command
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param sequence_number Sequence number
 * @param dsk DSK Key, can be NULL or 16 byte length
 * @param meta_data Meta data
 * @param metadata_length Meta data length
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_node_provisioning_report( znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     uint8_t sequence_number,
                                                     uint8_t* dsk,
                                                     uint8_t* meta_data,
                                                     uint8_t metadata_length );

/**
 * @brief Send Node Provisioning List Iteration report command
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param sequence_number Sequence number
 * @param remaining_count Remaining counter
 * @param dsk DSK Key, can be NULL or 16 byte length
 * @param meta_data Meta data
 * @param metadata_length Meta data length
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_node_provisioning_list_iteration_report( znet_node_id_t node_id,
                                                                    znet_node_channel_id_t channel_id,
                                                                    uint8_t sequence_number,
                                                                    uint8_t remaining_count,
                                                                    uint8_t* dsk,
                                                                    uint8_t* meta_data,
                                                                    uint8_t metadata_length );

/**
 * @brief Request the wake up interval.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_wakeup_interval_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set a device to a sleep mode.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_wakeup_sleep( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set the wake up interval value.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param wakeup_interval Wake up time interval
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_wakeup_interval_set( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_cmd_wakeup_value_t wakeup_interval );

/**
 * @brief Request the wake up capabilities.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_wakeup_capabilities_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat mode.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_mode_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set the thermostat mode.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Values
 * @param data Pointer to an array of bytes
 * @param data_len Number of bytes in @param data
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_mode_set( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_cmd_thermostat_mode_value_t value,
                                                const uint8_t* data,
                                                uint8_t data_len );

/**
 * @brief Request the thermostat's supported modes.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_mode_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat operating state.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_operating_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat operating state logging supported.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Values
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_operating_state_logging_supported_get( znet_node_id_t node_id,
                                                                             znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat's operating state logging.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param datalen Length of bit_mask array
 * @param bit_masks Point to Bit Masks' array
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_operating_state_logging_get( znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   uint8_t datalen,
                                                                   const uint8_t* bit_masks );

/**
 * @brief Request the thermostat fan mode.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_fan_mode_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set the thermostat fan mode.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Values
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_fan_mode_set( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_thermostat_fan_mode_value_t fan_mode );

/**
 * @brief Request the thermostat fan's supported modes.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_fan_mode_supported_get( znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat fan state.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_fan_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat setback.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_setback_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Set the thermostat setback.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Values
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_setback_set( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t type,
                                                   int8_t state );

/**
 * @brief Request the thermostat setpoint.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_setpoint_get( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_thermostat_setpoint_value_t type );

/**
 * @brief Set the thermostat setpoint.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param value Values
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_setpoint_set( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_thermostat_setpoint_value_t type,
                                                    znet_cmd_thermostat_setpoint_value_t size,
                                                    znet_cmd_thermostat_setpoint_value_t scale,
                                                    znet_cmd_thermostat_setpoint_value_t precision,
                                                    int32_t value );

/**
 * @brief Request the thermostat supported setpoints.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_setpoint_supported_get( znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id );

/**
 * @brief Request the thermostat setpoint capabilities for pointed type.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @type Type of setpoint
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_thermostat_setpoint_capabilities_get( znet_node_id_t node_id,
                                                                 znet_node_channel_id_t channel_id,
                                                                 znet_cmd_thermostat_setpoint_value_t type );

/**
 * @brief Request Indicator values.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param indicator_id Indicator ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_indicator_get( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t indicator_id );

znet_error_t znet_node_cmd_indicator_get_v1( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Request Indicator supported properties.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @param indicator_id Indicator ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_indicator_supported_get( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    uint8_t indicator_id );

/**
 * @brief Send Indicator Identify command.
 *
 * @param node_id Node ID
 * @param channel_id Channel ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_indicator_identify( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

/**
 * @brief Send DEVICE_RESET_LOCALLY notification
 *
 * @param node_id Node ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_cmd_device_reset_locally_notification( znet_node_id_t node_id );

znet_error_t znet_node_cmd_firmware_md_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

typedef struct znet_firmware_info_t znet_firmware_update_md_request_get_params_v1;

typedef struct znet_firmware_update_md_request_get_params_v3 {
    uint8_t firmware_target;
    uint16_t fragment_size;
} znet_firmware_update_md_request_get_params_v3;

typedef struct znet_firmware_update_md_request_get_params_v4 {
    bool is_postponed_activation_allowed;
} znet_firmware_update_md_request_get_params_v4;

typedef struct znet_firmware_update_md_request_get_params_v5 {
    uint8_t hardware_revision;
} znet_firmware_update_md_request_get_params_v5;

znet_error_t znet_node_cmd_firmware_update_md_request_get(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_firmware_update_md_request_get_params_v1* params_v1,
    const znet_firmware_update_md_request_get_params_v3* params_v3_opt,
    const znet_firmware_update_md_request_get_params_v4* params_v4_opt,
    const znet_firmware_update_md_request_get_params_v5* params_v5_opt );

znet_error_t znet_node_cmd_firmware_update_md_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t reports_count,
                                                   uint16_t report_number );

znet_error_t znet_node_cmd_firmware_update_md_report( znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint16_t report_number,
                                                      bool is_last_report,
                                                      uint8_t data_size,
                                                      const uint8_t* data,
                                                      bool use_checksum /*v2*/ );

typedef struct znet_firmware_update_activation_set_params_v4 {
    znet_firmware_info_t firmware;
    uint8_t firmware_target;
} znet_firmware_update_activation_set_params_v4;

typedef struct znet_firmware_update_activation_set_params_v5 {
    uint8_t hardware_version;
} znet_firmware_update_activation_set_params_v5;

znet_error_t znet_node_cmd_firmware_update_activation_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_firmware_update_activation_set_params_v4* params_v4,
    const znet_firmware_update_activation_set_params_v5* params_v5_opt );

znet_error_t znet_node_cmd_firmware_update_md_prepare_get( znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           uint16_t manufacturer_id,
                                                           uint16_t firmware_id,
                                                           uint8_t firmware_target,
                                                           uint16_t fragment_size,
                                                           uint8_t hardware_version );
/// INFO: utils

/**
 * @brief Calc node info size
 */
#define znet_node_info_size( node_info ) \
    ( sizeof( znet_nodeinfo_t ) + ( node_info )->commands_count * sizeof( znet_command_class_t ) )

/**
 * @brief Calc security supported commands size
 */
#define znet_security_supported_size( security_supported ) \
    ( sizeof( znet_security_supported_report_t ) +         \
      ( security_supported )->commands_count * sizeof( znet_command_class_t ) )

/**
 * @brief Check command is supported by node
 *
 * @param node_info Node info
 * @param command Command class
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 */
znet_error_t znet_node_info_is_support_command( const znet_nodeinfo_t* node_info, znet_command_class_t command );

/**
 * @brief Check command is supported by node
 *
 * @param node_id Node ID
 * @param command Command class
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 */
znet_error_t znet_node_is_support_command( znet_node_id_t node_id, znet_command_class_t command );

/**
 * @brief Check command is present in Nif
 *
 * @param node_id Node ID
 * @param enpoint_id enpoind
 * @param command Command class
 * @return Return true if command present in Nif.
 */
bool znet_node_is_have_command( znet_node_id_t node_id,
                                znet_node_channel_id_t endpoint_id,
                                znet_command_class_t command );

/**
 * @brief Check command is supported by node
 *
 * @param node_id Node ID
 * @return Return true if node is wakeup
 */
int znet_node_is_wakeup( znet_node_id_t node_id );

/**
 * @brief Check command is supported security by node
 *
 * @param node_info Node info
 * @param command Command class
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 */
znet_error_t znet_node_info_is_support_security_command( const znet_security_supported_report_t* security_supported,
                                                         znet_command_class_t command );

/**
 * @brief Get node info from cache
 *
 * Two mode:
 * If node_info is NULL then return recommended node_info_size
 * If node_info is noit NULL then return node info and actual node_info_size
 * size
 *
 * Example of use:
 * @code
 * size_t nis = 0;
 * size_t nis_tmp;
 * znet_nodeinfo_t* ni = NULL;
 * if( znet_node_info( 1, ni, &nis ) )
 * {
 *     // error
 *     exit();
 * }
 * ni = (znet_nodeinfo_t*)malloc( nis );
 * nis_tmp = nis;
 * if( znet_node_info( 1, ni, &nis_tmp ) )
 * {
 *     // error or not found
 *     exit();
 * }
 * // use ni
 * // optionally: recovery nis_tmp and get another node_info
 * free( ni );
 * @endcode
 *
 * If node info not found return error
 *
 * @param node_id Node ID
 * @param node_info Buffer for node info
 * @param node_info_size [IN] Buffer size [OUT] node info size
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 */

znet_error_t znet_node_info( znet_node_id_t node_id, znet_nodeinfo_t* node_info, size_t* node_info_size );

/**
 * @brief Get security supported commands from cache
 *
 * Two mode:
 * If security_supported is NULL then return recommended security_supported_size
 * If security_supported is noit NULL then return node info and actual
 * security_supported_size size
 *
 * Example of use:
 * @code
 * size_t sss = 0;
 * size_t sss_tmp;
 * znet_security_supported_report_t* ss = NULL;
 * if( znet_security_supported( 1, ss, &sss ) )
 * {
 *     // error
 *     exit();
 * }
 * ss = (znet_security_supported_report_t*)malloc( sss );
 * sss_tmp = sss;
 * if( znet_security_supported( 1, ss, &sss_tmp ) )
 * {
 *     // error or not found
 *     exit();
 * }
 * // use ss
 * // optionally: recovery sss_tmp and get another security_supported
 * free( ss );
 * @endcode
 *
 * If security supported commands not found return error
 *
 * @param node_id Node ID
 * @param security_supported Buffer for security supported commands
 * @param security_supported_size [IN] Buffer size [OUT] security supported
 * commands size
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success. On error, other value is returned.
 */

znet_error_t znet_security_supported( znet_node_id_t node_id,
                                      znet_security_supported_report_t* security_supported,
                                      size_t* security_supported_size );

/**
 * @brief Check if inclusion process of a node should be delegated to another controller
 *
 * @param node_id Node ID
 * @return Returns the result.
 */
bool znet_should_delegate_inclusion_to_sis( znet_node_id_t node_id );

/**
 * @brief Get a hub's home id
 *
 * @return Return a hub's home id
 */
znet_home_id_t znet_get_home_id();

/**
 * @brief Get a parent hub's node id
 *
 * @return Return a parent hub's node id
 */
znet_node_id_t znet_get_self_node_id();

/**
 * @brief Do we have the keys for security level
 *
 * @return Return a security levels
 */
bool znet_if_support_security_level( uint8_t security_level );

/**
 * @brief Get a SIS node id
 *
 * @return Return a SIS node id
 */
znet_node_id_t znet_get_sis_node_id();

bool is_self_node_sis_or_primary();

/**
 * @brief Get a hub's security level
 *
 * @return Return a hub`s security level
 */
uint8_t znet_get_my_highest_security();

/**
 * @brief Set a Inclusion Controller node id and security with wich we receive initiate message
 *
 * @return
 */
void znet_set_ic_info( znet_node_id_t id, uint8_t sec );

/**
 * @brief Get a Inclusion Controller node id and security with wich we receive initiate message
 *
 * @return Return a Inclusion Controller info
 */
inclusion_controller_info_t znet_get_ic_info();

/**
 * @brief Get private S2 DSK
 *
 */
znet_error_t znet_get_private_s2_dsk();

/**
 * @brief Create timer for forced requests
 *
 * @return Return an error status
 */
znet_error_t znet_forced_request_timer_create( int destroy );

/**
 * @brief Initiate device inclusion
 *
 * @param ic_info inclusion controller info
 * @param node_number Node ID
 * @param step_id Step ID
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_inclusion_initiate( inclusion_controller_info_t ic_info,
                                           uint8_t node_number,
                                           znet_inclution_controller_step_id_t step_id );

/**
 * @brief Complete device inclusion
 *
 * @param node_number Node ID
 * @param step_id Step ID
 * @param status Status
 * @return Return ZNET_ERR_OK or ZNET_ERR_REQUEST_SENT_OK on success if the task is queued for execution. On error,
 * other value is returned.
 */
znet_error_t znet_node_inclusion_complete( inclusion_controller_info_t ic_info,
                                           znet_inclution_controller_step_id_t step_id,
                                           znet_inclution_controller_status_t status );

/**
 * @brief Start S0 bootstrapping
 *
 * @param node_id Node ID
 * @param static_contoller node is
 */
void znet_node_start_S0( uint8_t node_id, bool static_contoller );

/**
 * @brief Start S2 bootstrapping
 *
 * @param node_id Node ID
 */
void znet_node_start_S2( uint8_t node_id );

/**
 * @brief znet_utils_get_security_encap_type_by_node_id
 *
 * @param node_id Node ID
 * @return Encapsulation type
 */
uint8_t znet_utils_get_security_encap_type_by_node_id( znet_node_id_t node_id );

znet_error_t znet_open_backup_session();

znet_error_t znet_write_backup_data( uint16_t offset, uint8_t dataLength, const uint8_t* data );

znet_error_t znet_read_backup_data( uint16_t offset, uint8_t dataLength );

znet_error_t znet_close_backup_session();

znet_error_t znet_node_cmd_anti_theft_set( znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           bool enabled,
                                           uint8_t magic_code_size,
                                           const uint8_t* magic_code,
                                           uint16_t manufacturer_id,
                                           uint8_t hint_size,
                                           const uint8_t* hint,
                                           uint16_t zwave_locking_id );

znet_error_t znet_node_cmd_anti_theft_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

znet_error_t znet_node_cmd_anti_theft_unlock_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

znet_error_t znet_node_cmd_anti_theft_unlock_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id );

znet_error_t znet_node_cmd_anti_theft_unlock_set( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t magic_code_size,
                                                  const uint8_t* magic_code );

#ifdef __cplusplus
}
#endif

#endif  // ZNET_H
