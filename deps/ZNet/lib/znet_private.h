#ifndef __ZNET_PRIVATE_H__
#define __ZNET_PRIVATE_H__

#include "ZErrors.h"

/// TODO: move to config.h file
#define ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES 1

// Broadcast Node Id
#define NODE_BROADCAST 0xFF

// Maximum allowed Node Id
#define NODE_MAX 0xE8

#define MAGIC_LEN 29

struct _ZNet;
typedef struct _ZNet* ZNet;
typedef const struct _ZNet* ZNetConst;
//#define ZNet struct _ZNet*

// struct _ZDevice;
// typedef struct _ZDevice* ZDevice;
//#define ZDevice  struct _ZDevice*

// struct _ZInstance;
// typedef struct _ZInstance* ZInstance;
//#define ZInstance  struct _ZInstance*

struct _ZFunctionClass;
// typedef struct _ZFunctionClass* ZFunctionClass;
#define ZFunctionClass struct _ZFunctionClass*

struct _ZFunction;
#define ZFunction struct _ZFunction*

struct _ZTimer;
#define ZTimer struct _ZTimer*

struct _ZMessage;
#define ZMessage struct _ZMessage*

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
struct _ZWaitingCommand;
#define ZWaitingCommand struct _ZWaitingCommand*
#endif

// struct _ZCommandClass;
// typedef struct _ZCommandClass* ZCommandClass;
//#define ZCommandClass  struct _ZCommandClass*

// struct _ZCommand;
// typedef struct _ZCommand* ZCommand;
//#define ZCommand  struct _ZCommand*

// struct _ZNonce;
// typedef struct _ZNonce* ZNonce;
//#define ZNonce  struct _ZNonce*

// struct _ZJob;
// typedef struct _ZJob* ZJob;
//#define ZJob struct _ZJob*
// struct _ZJobList;
// typedef struct _ZJobList* ZJobList;
//#define ZJobList  struct _ZJobList*

// ZCommand ICACHE_FLASH_ATTR ZNet_GetCommand( const  uint8_t node_id,
// uint8_t instance_id, uint8_t command_id );

// add a device if not exists, and optionally update type and NIF
// - if node_type == -1, node type is not updated
// - if nif_length == -1, NIF is not updated
// ZDevice ICACHE_FLASH_ATTR _znet_update_device( uint8_t node_id,
// int node_type, int nif_length, const uint8_t* nif );
ZWError ICACHE_FLASH_ATTR _znet_controller_assign_suc( uint8_t node_id );
void ICACHE_FLASH_ATTR ZNet_GetMandatoryCCes( const ZNet znet,
                                              uint8_t genericType,
                                              uint8_t specificType,
                                              uint8_t* cc_mask );

#endif  // #ifndef __ZNET_PRIVATE_H__
