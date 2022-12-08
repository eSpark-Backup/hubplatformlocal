#ifndef __FC_SENDDATA_H__
#define __FC_SENDDATA_H__

#include "typedefs.h"

#include "znet.h"
//
#include "ZErrors.h"

typedef struct {
    uint8_t length;
    const uint8_t* command;
} BundledCommand_t;

typedef struct {
    BundledCommand_t* bundle;
    uint8_t commandIdx;
} SendDataContext_t;

extern SendDataContext_t send_data_context;

FuncResults_e ICACHE_FLASH_ATTR __SendDataResponse( ZFunction func );
FuncResults_e ICACHE_FLASH_ATTR __SendDataCallback( ZFunction func );
FuncResults_e ICACHE_FLASH_ATTR __SendDataTimeout( ZFunction func );

typedef struct _SendDataInfo {
    uint8_t _node_id;
    uint8_t _length;
    const uint8_t* _data;

} SendDataInfo_t;
typedef SendDataInfo_t* SendDataInfo;

#endif  // #ifndef __FC_SENDDATA_H__
