
#ifndef __ZNET_FUNCTION_CLASS_H__
#define __ZNET_FUNCTION_CLASS_H__

//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "buffers.h"

#include "ZErrors.h"
#include "znet_private.h"
#include "znet/znet_errors.h"

#include "system_time.h"
#include "lists.h"

//////////////////////////////////////////////////////////////////////////////

// struct _ZFunction;
// typedef struct _ZFunction* ZFunction;

#define FUNCTION_CLASS_REQ_ATTEMPTS_COUNT 3
//#define FUNCTION_CLASS_REQ_TIMEOUT_MS       20000
#define FUNCTION_CLASS_ACK_TIMEOUT_MS 1500
#define FUNCTION_CLASS_DELAY_TIMEOUT_MS 500
//#define FUNCTION_CLASS_RESPONSE_TIMEOUT_MS  20000
//#define FUNCTION_CLASS_CALLBACK_TIMEOUT_MS  20000
#define FUNCTION_CLASS_TIMEOUT_100_MS 100
#define FUNCTION_CLASS_TIMEOUT_500_MS 500
#define FUNCTION_CLASS_TIMEOUT_1_S 1000
#define FUNCTION_CLASS_TIMEOUT_2_S 2000
#define FUNCTION_CLASS_TIMEOUT_3_S 3000
#define FUNCTION_CLASS_TIMEOUT_5_S 5000
#define FUNCTION_CLASS_TIMEOUT_20_S 20000
#define FUNCTION_CLASS_TIMEOUT_60_S 60000
#define FUNCTION_CLASS_TIMEOUT_1_MIN 60000

typedef enum {
    FuncResults_Done = 0,     // a func finished completely
    FuncResults_Restart = 1,  // start full cycle from the beginning (send to TX queue)
    FuncResults_Repeat = 2,   // once more this stage
    FuncResults_Next = 3      // goto next stage of a func
} FuncResults_e;

typedef znet_error_t ( *ZFunctionInit )( ZFunction function, void* initArg );
typedef FuncResults_e ( *ZFunctionProcessACK )( ZFunction function );
typedef FuncResults_e ( *ZFunctionProcessRESPONSE )( ZFunction function );
typedef FuncResults_e ( *ZFunctionProcessCALLBACK )( ZFunction function );
// typedef FuncResults_e (*ZFunctionProcessTimeout      )( ZFunction function );
typedef bool ( *ZFunctionIsCallbackFrameMine )( ZFunction function );

typedef struct _ZFunctionClass {
    const uint8_t _id;
    const char* const _name;
    // const char*                  const _description;

    ZFunctionInit _Init;
    ZFunctionProcessACK _ProcessACK;
    ZFunctionProcessRESPONSE _ProcessRESPONSE;
    ZFunctionProcessCALLBACK _ProcessCALLBACK;
    // ZFunctionProcessTimeout      _ProcessTimeout;
    ZFunctionIsCallbackFrameMine _IsCallbackFrameMine;

    // bool                         _urgent         /*:1*/;
    // bool                         _block          /*:1*/;
    // bool                         _allow_duplicate/*:1*/;
    // bool                         _sleeping_check /*:1*/;
    // bool                         _security       /*:1*/;
    // bool                         _has_node       /*:1*/;
    // bool                         _send_to_node   /*:1*/;

    // additional timeouts
    // float                        _timeout_response;
    // float                        _timeout_callback_first;
    // float                        _timeout_callback_more;

} ZFunctionClass_t;

// returns a known function class by id
const ZFunctionClass ICACHE_FLASH_ATTR ZFunctionClass_GetById( uint8_t id );
// returns a known function class by name
// const ZFunctionClass ICACHE_FLASH_ATTR ZFunctionClass_GetByName( const char*
// name );
// returns a supported function class, or NULL if not supported
// const ZFunctionClass ICACHE_FLASH_ATTR ZFunctionClass_GetSupportedById(const
//  uint8_t id);

void ICACHE_FLASH_ATTR ZFunctionClass_FillBySupportedFC( uint8_t* bitmask );

//////////////////////////////////////////////////////////////////////////////

typedef enum {
    FUNC_OK = 0x0000,
    FUNC_FAILURE_ADD_TO_TX = 0xF001,
    FUNC_FAILURE_ADD_TO_ACK = 0xF002,
    FUNC_FAILURE_ADD_TO_RESPONSE = 0xF003,
    FUNC_FAILURE_ADD_TO_CALLBACK = 0xF004,
    FUNC_FAILURE_TIMEOUT = 0xF005,
    FUNC_FAILURE_RXED_BUFFER_TOO_SHORT = 0xF006,
    FUNC_FAILURE_RESPONSE_SIZE = 0xF007,
    FUNC_FAILURE_RESPONSE_INVALID = 0xF008,
    FUNC_FAILURE_CALLBACK_SIZE = 0xF009,
    FUNC_FAILURE_CALLBACK_INVALID = 0xF00A,
    FUNC_FAILURE_NOMEM = 0xF00B,
    FUNC_FAILURE_NO_ACK = 0xF00C,
    FUNC_FAILURE_NETWORK_BUSY = 0xF00D,  // not possible to transmit data because network is busy
    FUNC_FAILURE_GENERAL = 0xFFFF
} ZFuncFailures_e;

typedef void (
    *ZFunction_Callback )( ZFunction func, void* arg, ZFuncFailures_e reason, const void* dataPtr, size_t dataSize );

typedef struct _ZFunction {
    ZNet _znet;
    ZFunctionClass const _fc;

    CyclicBuffer_t _tx_frame;
    int _tx_frame_initial_len;

    SystemTime_t _timeout;
    SystemTime_t _timeout_delta;
    int _tx_attempt_counter;
    // uint8_t                         _req_attempts;

    ZFunctionProcessRESPONSE _ProcessRESPONSE;
    ZFunctionProcessCALLBACK _ProcessCALLBACK;

    ZFunction_Callback const _Callback;
    void* _callbackArg;

    uint8_t _security;
    uint8_t _endpoint;
    uint8_t* _dataPtr;
    uint8_t _dataSize;
    uint8_t _node_id;
} ZFunction_t;

ZFunction ICACHE_FLASH_ATTR ZFunction_Create( const ZFunctionClass const fc,
                                              void* initArg,
                                              ZFunction_Callback const Callback,
                                              void* callbackArg );
void ICACHE_FLASH_ATTR ZFunction_Free( void* func );
void ICACHE_FLASH_ATTR ZFunction_Free_From_All_List( const ZFunctionClass const fc );
List_Entry ICACHE_FLASH_ATTR ZFunction_Find_Function_in_All_List( const ZFunctionClass const fc );
List_Entry ICACHE_FLASH_ATTR ZFunction_Find_Function_in_All_List_By_Init_Arg( List_CheckObjectFunc CheckFunc,
                                                                              const void* checkObjectFuncArg );
bool ICACHE_FLASH_ATTR ZFunction_Check_is_Lists_Empty();

bool ICACHE_FLASH_ATTR CheckFuncToNodeID( const void* object, const void* arg );

#endif  // #ifndef __ZNET_FUNCTION_CLASS_H__
