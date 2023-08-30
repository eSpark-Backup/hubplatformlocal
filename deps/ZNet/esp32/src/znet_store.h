/**
 * @file znet_store.h
 * @date 16 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_STORE_H
#define ZNET_STORE_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <znet/znet.h>
#include <znet/znet_errors.h>
#include "znet_lib_cc.h"

/**
 * @brief Block size
 */
#define ZNET_STORE_HEAD_SIZE_MAX 64
#define ZNET_STORE_NODE_INFO_SIZE_MAX 64
#define ZNET_STORE_S2_KEYSTORE_SIZE_MAX 128
#define ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX 64

/**
 * @brief Store init
 *
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_init( void );

/**
 * @brief Store reset
 *
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_reset( void );

/**
 * @brief Save new or full update node info or update only NIF
 *
 * @param node_info Node info
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_node_info_set( const znet_nodeinfo_t* node_info );

/**
 * @brief Get node info
 *
 * @param node_id Node ID
 * @param node_info Buffer for node info
 * @param size Buffer size
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_node_info_get( znet_node_id_t node_id, znet_nodeinfo_t* node_info, size_t size );

/**
 * @brief Clean node info by node ID
 *
 * @param node_id Node ID
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_node_info_clean( znet_node_id_t node_id );

/**
 * @brief Get recommended size for znet_nodeinfo_t
 *
 * @return Return recommended size for znet_nodeinfo_t
 */
#define znet_store_node_info_recommended_size() ( ZNET_STORE_NODE_INFO_SIZE_MAX )

/**
 * @brief Save new or update security supported commands
 *
 * @param node_id Node ID
 * @param security_supported Security supported commands
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_security_supported_set( znet_node_id_t node_id,
                                                const znet_security_supported_report_t* security_supported,
                                                size_t follow_parts );

/**
 * @brief Get update security supported commands
 *
 * @param node_id Node ID
 * @param security_supported Buffer for security supported commands
 * @param size Buffer size
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_security_supported_get( znet_node_id_t node_id,
                                                znet_security_supported_report_t* security_supported,
                                                size_t size );

/**
 * @brief Clean security supported commands by node ID
 *
 * @param node_id Node ID
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_security_supported_clean( znet_node_id_t node_id );

/**
 * @brief Get recommended size for znet_security_supported_report_t
 *
 * @return Return recommended size for znet_security_supported_report_t
 */
#define znet_store_security_supported_recommended_size() ( ZNET_STORE_SECURITY_SUPPORTED_SIZE_MAX - sizeof( uint8_t ) )

/**
 * @brief Save S2_keystore
 *
 * @param data Buffer with data
 * @param size Buffer size
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_S2_keystore_set( void* data, size_t size );

/**
 * @brief Get S2_keystore
 *
 * @param data Buffer for data
 * @param size Buff size for data
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_S2_keystore_get( void* data, size_t size );

/**
 * @brief Save association
 *
 * @param group_id Group ID
 * @param associations All node IDs in the group
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_associations_set( uint8_t group_id, const uint8_t* associations );

/**
 * @brief Get all associations for a group ID
 *
 * @param group_id Group ID
 * @param associations All node IDs in the group
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_associations_get( uint8_t group_id, uint8_t* associations );

/**
 * @brief Save association
 *
 * @param group_id Group ID
 * @param associations All node IDs with properties in the group
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_multichannel_associations_set( uint8_t group_id,
                                                       const VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG* associations );

/**
 * @brief Get all associations for a group ID
 *
 * @param group_id Group ID
 * @param associations All node IDs with properties in the group
 * @return Return zero on success. On error, error code is returned
 */
znet_error_t znet_store_multichannel_associations_get( uint8_t group_id,
                                                       VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG* associations );

#endif  // ZNET_STORE_H
