
#ifndef __ZNET_FC_PRIVATE_H__
#define __ZNET_FC_PRIVATE_H__

#include "ZFunctionClass.h"

extern const ZFunctionClass_t fcDummyFunctionClass;
extern const ZFunctionClass_t fcGetSerialAPICapabilities;
extern const ZFunctionClass_t fcSerialAPISoftReset;
extern const ZFunctionClass_t fcSerialAPISetTimeouts;
extern const ZFunctionClass_t fcWatchDogStart;
extern const ZFunctionClass_t fcWatchDogStop;
extern const ZFunctionClass_t fcGetHomeId;
extern const ZFunctionClass_t fcGetControllerCapabilities;
extern const ZFunctionClass_t fcGetVersion;
extern const ZFunctionClass_t fcSerialAPIGetInitData;
extern const ZFunctionClass_t fcSerialAPIApplicationNodeInfo;
extern const ZFunctionClass_t fcSerialAPISoftReset;
extern const ZFunctionClass_t fcEnableSUC;
extern const ZFunctionClass_t fcSetSUCNodeId;
extern const ZFunctionClass_t fcApplicationCommandHandler;
extern const ZFunctionClass_t fcApplicationCommandHandlerBridge;
extern const ZFunctionClass_t fcApplicationNodeUpdate;
extern const ZFunctionClass_t fcMemoryGetByte;
extern const ZFunctionClass_t fcMemoryGetBuffer;
extern const ZFunctionClass_t fcMemoryPutByte;
extern const ZFunctionClass_t fcMemoryPutBuffer;
extern const ZFunctionClass_t fcIsFailedNode;
extern const ZFunctionClass_t fcSendDataAbort;
extern const ZFunctionClass_t fcSendData;
extern const ZFunctionClass_t fcSendDataBridge;
extern const ZFunctionClass_t fcSendDataMulti;
extern const ZFunctionClass_t fcSendDataMultiBridge;
extern const ZFunctionClass_t fcGetNodeProtocolInfo;
extern const ZFunctionClass_t fcGetRoutingTableLine;
extern const ZFunctionClass_t fcAssignReturnRoute;
extern const ZFunctionClass_t fcAssignSUCReturnRoute;
extern const ZFunctionClass_t fcDeleteReturnRoute;
extern const ZFunctionClass_t fcDeleteSUCReturnRoute;
extern const ZFunctionClass_t fcSetDefault;
extern const ZFunctionClass_t fcGetSUCNodeId;
extern const ZFunctionClass_t fcSendSUCNodeId;
extern const ZFunctionClass_t fcSendNodeInformation;
extern const ZFunctionClass_t fcRequestNodeInformation;
extern const ZFunctionClass_t fcRequestNodeNeighborUpdate;
extern const ZFunctionClass_t fcRemoveFailedNode;
extern const ZFunctionClass_t fcReplaceFailedNode;
extern const ZFunctionClass_t fcRequestNetworkUpdate;
extern const ZFunctionClass_t fcRequestNodeNeighbourUpdate;
extern const ZFunctionClass_t fcSetLearnMode;
extern const ZFunctionClass_t fcAddNodeToNetwork;
extern const ZFunctionClass_t fcRemoveNodeFromNetwork;
extern const ZFunctionClass_t fcControllerChange;
extern const ZFunctionClass_t fcCreateNewPrimary;
extern const ZFunctionClass_t fcZMEFreqChange;
extern const ZFunctionClass_t fcZMERestore;
extern const ZFunctionClass_t fcSendTestFrame;
extern const ZFunctionClass_t fcLibraryVersion;
extern const ZFunctionClass_t fcTypeLibrary;
extern const ZFunctionClass_t fcRFPowerLevelGet;
extern const ZFunctionClass_t fcRFPowerLevelSet;
extern const ZFunctionClass_t fcStoreHomeId;
extern const ZFunctionClass_t fcExploreRequestInclusion;
extern const ZFunctionClass_t fcSetRoutingMAX;
extern const ZFunctionClass_t fcNVRGetValue;
extern const ZFunctionClass_t fcNvmBackupRestore;
extern const ZFunctionClass_t fcSerialAPIStarted;
extern const ZFunctionClass_t fcSetThreshold;
extern const ZFunctionClass_t fcGetBackGroundRSSI;

#define TRANSMIT_OPTION_NONE 0x00
#define TRANSMIT_OPTION_ACK 0x01
#define TRANSMIT_OPTION_LOW_POWER 0x02
#define TRANSMIT_OPTION_AUTO_ROUTE 0x04
#define TRANSMIT_OPTION_FORCE_ROUTE 0x08
#define TRANSMIT_OPTION_NO_ROUTE 0x10
#define TRANSMIT_OPTION_EXPLORE 0x20
#define TRANSMIT_OPTION_NO_RETRANSMIT 0x40

#define TRANSMIT_COMPLETE_OK 0x00
#define TRANSMIT_COMPLETE_NO_ACK 0x01
#define TRANSMIT_COMPLETE_FAIL 0x02
#define TRANSMIT_COMPLETE_NOROUTE 0x04

#define SEND_DATA_NONE 0x00
#define SEND_DATA_WAIT_SECURITY 0x01
#define SEND_DATA_ALLOW_DUPLICATE 0x02
#define SEND_DATA_URGENT 0x04

// ZWError ICACHE_FLASH_ATTR _znet_check_failed_node( ZDevice device,
// bool shouldFail);  znet_error_t ICACHE_FLASH_ATTR FrameIsNotDelivered( ZNet znet );
// ZWError ICACHE_FLASH_ATTR SendData( uint8_t node_id, uint8_t
// instance_id, uint8_t length, const uint8_t* data, const char* description,
// ZJobList onBehalfOf, uint8_t flags,
// ZFunction_Callback Callback, void* callbackArg );

#define ZME_RESTORE_HOMEID_FIRST 251
#define ZME_RESTORE_HOMEID_SECOND 252
#define ZME_RESTORE_MAXNODE 255

// ZWError ICACHE_FLASH_ATTR znet_fc_zme_restore( uint8_t cmdId,
// uint8_t length, const uint8_t* data,
// ZFunction_Callback Callback, void* callbackArg );

#endif  // #ifndef __ZNET_FC_PRIVATE_H__
