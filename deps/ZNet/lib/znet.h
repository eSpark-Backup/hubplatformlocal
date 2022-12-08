
#ifndef __ZNET_H__
#define __ZNET_H__

//////////////////////////////////////////////////////////////////////////////

#include "lists.h"
#include "serial_port.h"
#include "ZErrors.h"
#include "znet_private.h"
#include "znet/znet_errors.h"
#include "znet_release_info.h"
//#include "ZControllerData.h"
//
#include "ZFunctionClass.h"
#include "ZTrackedTimes.h"

//////////////////////////////////////////////////////////////////////////////

// misc I/O flags
#define REQUEST 0x00
#define RESPONSE 0x01

// MAX_PACKET_SIZE = 255 + length + packet_type
#define MAX_PACKET_SIZE 257

//----------------------------------------------------------------------------
enum _FrameTypes { SOF = 0x01, ACK = 0x06, NAK = 0x15, CAN = 0x18 };

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

// Definition of termination callback function
typedef void ( *ZTerminationCallback )( const ZNet znet );

typedef struct _ZNet {
    int _ticks;

    SerialPort _serialPort;
    bool _serial_api_ininitalized;  // flag if GetSerialAPICapabilities was
                                    // finished
    uint8_t _rxedBufferData[MAX_PACKET_SIZE];
    CyclicBuffer_t _rxedBuffer;
    uint8_t _frameData[MAX_PACKET_SIZE];
    Buffer_t _frame;
    uint8_t _frameLenToRX;

    //    Activity_t _rxActivity;
    volatile bool _is_running;                   // running flag
    ZTerminationCallback _termination_callback;  // callback on working thread termination

    //    Activity_t _binarySwitchingActivity;

    uint16_t _vendor_id;

    uint8_t _supported_function_classes[32];  // bitmask for supported function
                                              // classes
    // ZControllerData_t    _controllerData;

    // ZList_t               _queue;

    // ZList_t               _devices;
    // TrackedTimes_t       _devices_times;

    // ZList_t               _nonces;

    ZList_t _device_callbacks;

    uint8_t _callback_id;

    // float                _timeout_channel;
    // float                _timeout_ack;
    // float                _timeout_response;
    // float                _timeout_callback_first;
    // float                _timeout_callback_more;
    // float                _timeout_done;

    // int                  _max_resend_count: 5;
    // int                  _vendor_value    :16;
    // int                  _vendor_mask     : 3;

    // bool                 _try_to_become_sis /*:1*/;
    // bool                 _discovery_finished/*:1*/;
    // bool                 _discovery_error   /*:1*/;
    // bool                 _devices_loaded    /*:1*/;
    // bool                 _devices_saving    /*:1*/;

    ZFunction _func_to_TX;

    SystemTime_t _timeout;
    /**
     * A timepoint that indicates when processing of tx queue will begin.
     * The field allows to postpone processing if set to a timepoint in the future.
     */
    SystemTime_t _postpone_sending_time;
    ZList_t _funcs_to_TX;
    ZList_t _funcs_to_ACK;
    ZList_t _funcs_to_RESPONSE;
    ZList_t _funcs_to_CALLBACK;

    ZList_t _timers;
    ZList_t _security_message;

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
    uint8_t _waking_nodes[32];  // bitmask
    ZList_t _waiting_commands;
#endif
} ZNet_t;

extern ZNet_t znet_instance;

typedef void ( *ZTimer_Callback )( ZTimer func, void* arg, int destroy );

typedef struct _ZTimer {
    ZNet const _znet;

    SystemTime_t _timeout;

    ZTimer_Callback const _Callback;
    void* _callbackArg;
} ZTimer_t;

typedef struct _ZMessage {
    ZNet const _znet;

    void* _message;
    uint8_t _length;
    uint8_t _node_id;
    ZFunction_Callback _callback;
    void* _callbackArg;
    uint8_t _encap;
} ZMessage_t;

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
typedef struct _ZWaitingCommand {
    ZNet _znet;

    uint8_t _func_class_id;

    uint8_t _node_id;
    uint8_t _channel_id;
    ZFunction_Callback _cb;
    void* _cb_arg;
    encap_type_t _encap;
    size_t _data_size;
    uint8_t _data[];
} ZWaitingCommand_t;
#endif

ZTimer ICACHE_FLASH_ATTR ZTimer_Create( SystemTime_t timeout, ZTimer_Callback const Callback, void* callbackArg );
void ICACHE_FLASH_ATTR ZTimer_Free( void* timer );

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
znet_error_t ICACHE_FLASH_ATTR ZWaitingCommand_Call( ZWaitingCommand waiting_command );
ZWaitingCommand ICACHE_FLASH_ATTR ZWaitingCommand_Create( uint8_t func_class_id,
                                                          uint8_t node_id,
                                                          uint8_t channel_id,
                                                          ZFunction_Callback cb,
                                                          void* cb_arg,
                                                          encap_type_t encap,
                                                          const void* data,
                                                          size_t data_size );
void ICACHE_FLASH_ATTR ZWaitingCommand_Free( void* waiting_command );
#endif

ZWError ICACHE_FLASH_ATTR ZNet_Init( SerialPort serialPort );
bool ICACHE_FLASH_ATTR ZNet_DeInit( ZNet znet );

// ZNonce ICACHE_FLASH_ATTR ZNet_AppendNonce    (        const
// uint8_t* data );  ZNonce ICACHE_FLASH_ATTR ZNet_GetNonceById   ( const ZNet
// znet, uint8_t id );  ZNonce ICACHE_FLASH_ATTR ZNet_LookupNonce    ( const
//  uint8_t id );  bool   ICACHE_FLASH_ATTR ZNet_RemoveNonce    (
//  ZNonce nonce );  bool   ICACHE_FLASH_ATTR ZNet_RemoveNonceById(
//  uint8_t id );

// FC ---> FunctionClass
// checks if function class is supported
bool ICACHE_FLASH_ATTR ZNet_IsFCSupported( const uint8_t id );

/// job queue processing
void ICACHE_FLASH_ATTR ZNet_RecalcDevicesJobQueueLen( const ZNet znet );
// ZJob ICACHE_FLASH_ATTR ZNet_LastJob( const ZNet znet );
// add job to queue
// ZWError ICACHE_FLASH_ATTR ZNet_AddJob( ZNet znet );
// remove job from queue
// ZWError ICACHE_FLASH_ATTR ZNet_RemoveJob( const ZJob job ); ///
// !!! called from nowhere !!! remove job from queue (unsafe version)
// ZWError ICACHE_FLASH_ATTR ZNet_RemoveJobUnsafe( const ZJob job);
// remove jobs for specific node from queue
void ICACHE_FLASH_ATTR ZNet_RemoveJobForNode( uint8_t node_id );
// returns a job awaiting for ACK, or NULL if there's no one
// ZJob ICACHE_FLASH_ATTR ZNet_FindAwaitingAckJob( const ZNet znet );
// returns a job awaiting response, or NULL if there's no one
// ZJob ICACHE_FLASH_ATTR ZNet_FindAwaitingResponseJob( const  uint8_t
// functionId );
// returns a job awaiting callback, or NULL if there's no one
// ZJob ICACHE_FLASH_ATTR ZNet_FindAwaitingCallbackJob( const  uint8_t
// functionId, uint8_t callbackId );
// returns jobs as a job list, optionally filtered
// ZJobList ICACHE_FLASH_ATTR ZNet_CreateJobListByFilter(
// ZJobListFilter filter );
// returns if more packets are waiting for security to same node is this job
// bool ICACHE_FLASH_ATTR ZNet_HasMoreSecurePacketJobs( const  const
// ZJob job);  ZJob ICACHE_FLASH_ATTR ZNet_FindDeviceFunctionLastJob( const ZNet
// znet, uint8_t deviceId, uint8_t functionId );

void ICACHE_FLASH_ATTR ZNet_ProcessACK();
void ICACHE_FLASH_ATTR ZNet_ProcessNAKandCAN();
void ICACHE_FLASH_ATTR ZNet_ProcessSOF();
void ICACHE_FLASH_ATTR ZNet_ProcessTimeout();

znet_error_t ICACHE_FLASH_ATTR ZNet_AddFuncToTX( ZFunction func );
znet_error_t ICACHE_FLASH_ATTR ZNet_AddFuncToTX_timeout( ZFunction func, uint32_t timeout );
void ICACHE_FLASH_ATTR ZNet_AddFuncToACK( ZFunction func );
void ICACHE_FLASH_ATTR ZNet_RefreshTimeout();

void ICACHE_FLASH_ATTR ZNet_ProcessTimers();
bool ICACHE_FLASH_ATTR ZNet_AddTimer( ZTimer timer );
bool ICACHE_FLASH_ATTR ZNet_RemoveTimer( ZTimer timer );

void ICACHE_FLASH_ATTR ZNet_RemoveAllFuncToTxForNode( const uint8_t node_id );

ZMessage ICACHE_FLASH_ATTR ZMessage_Create( const uint8_t* data,
                                            uint8_t data_length,
                                            uint8_t node_id,
                                            uint8_t encap,
                                            ZFunction_Callback callback,
                                            void* callbackArg );
void ICACHE_FLASH_ATTR ZMessage_Free( void* message );
bool ICACHE_FLASH_ATTR ZNet_Add_Security_Message( ZMessage message );
void ICACHE_FLASH_ATTR ZNet_Remove_Security_Message( ZMessage message );

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
void ICACHE_FLASH_ATTR ZNet_ProcessWaitingCommands( uint8_t node_id );
znet_error_t ICACHE_FLASH_ATTR ZNet_AddWaitingCommand( uint8_t func_class_id,
                                                       uint8_t node_id,
                                                       uint8_t channel_id,
                                                       ZFunction_Callback cb,
                                                       void* cb_arg,
                                                       encap_type_t encap,
                                                       const void* data,
                                                       size_t data_size );
void ICACHE_FLASH_ATTR ZNet_WakingNode( uint8_t node_id, int waking );
int ICACHE_FLASH_ATTR ZNet_IsWakingNode( uint8_t node_id );
#endif

// void ICACHE_FLASH_ATTR znet_queue_inspect( const  FILE * stream );

encap_type_t ICACHE_FLASH_ATTR get_security_by_id( uint8_t node_id );
encap_type_t ICACHE_FLASH_ATTR get_security_by_id_and_cc( uint8_t node_id, uint8_t channel, uint8_t cc );
bool is_node_added( uint8_t node_id );

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
int ICACHE_FLASH_ATTR node_is_wakeup( uint8_t node_id );
#endif

#endif  // #ifndef __ZNET_H__
