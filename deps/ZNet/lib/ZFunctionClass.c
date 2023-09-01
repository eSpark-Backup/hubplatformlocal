#include <stdlib.h>

// Last entry should be NULL
// For performance reasons, those function classes are used frequently should go
// first

#include "ZPlatform.h"

#include "array_bin_search.h"

#include "byte_ops.h"

#include "znet.h"
#include "znet_fc_private.h"

extern uint8_t MY_NODE_ID;
//----------------------------------------------------------------------------
static const ZFunctionClass const __FunctionClassesById[] = {
    /*0x02*/ &fcSerialAPIGetInitData,
    /*0x03*/ &fcSerialAPIApplicationNodeInfo,
    /*0x04*/ &fcApplicationCommandHandler,
    /*0x07*/ &fcGetSerialAPICapabilities,
    /*0x08*/ &fcSerialAPISoftReset,
    /*0x0A*/ &fcSerialAPIStarted,
    /*0x12*/ &fcSendNodeInformation,
    /*0x13*/ &fcSendData,
    /*0x14*/ &fcSendDataMulti,
    /*0x15*/ &fcLibraryVersion,
    /*0x17*/ &fcRFPowerLevelSet,
    /*0x20*/ &fcGetHomeId,
    /*0x28*/ &fcNVRGetValue,
    /*0x2E*/ &fcNvmBackupRestore,
    /*0x3B*/ &fcGetBackGroundRSSI,
    /*0x3C*/ &fcSetThreshold,
    /*0x41*/ &fcGetNodeProtocolInfo,  // GetNodeProtocolInformation
    /*0x42*/ &fcSetDefault,
    /*0x48*/ &fcRequestNodeNeighborUpdate,
    /*0x49*/ &fcApplicationNodeUpdate,
    /*0x4a*/ &fcAddNodeToNetwork,
    /*0x4b*/ &fcRemoveNodeFromNetwork,
    /*0x50*/ &fcSetLearnMode,
    /*0x51*/ &fcAssignSUCReturnRoute,
    /*0x53*/ &fcRequestNetworkUpdate,
    /*0x54*/ &fcSetSUCNodeId,
    /*0x56*/ &fcGetSUCNodeId,
    /*0x57*/ &fcSendSUCNodeId,
    /*0x5E*/ &fcExploreRequestInclusion,
    /*0x60*/ &fcRequestNodeInformation,
    /*0x61*/ &fcRemoveFailedNode,
    /*0x62*/ &fcIsFailedNode,
    /*0x63*/ &fcReplaceFailedNode,
    /*0x84*/ &fcStoreHomeId,
    /*0xA8*/ &fcApplicationCommandHandlerBridge,
    /*0xA9*/ &fcSendDataBridge,
    /*0xAB*/ &fcSendDataMultiBridge,
    /*0xBA*/ &fcRFPowerLevelGet,
    /*0xBD*/ &fcTypeLibrary,
    /*0xBE*/ &fcSendTestFrame,
    /*0xD4*/ &fcSetRoutingMAX
};

//----------------------------------------------------------------------------

static int ICACHE_FLASH_ATTR ZFunctionClass_CmpById( const void* key, const int index ) {
    return ( (int)( *(uint8_t*)key ) - (int)__FunctionClassesById[index]->_id );
}

// const
const ZFunctionClass ICACHE_FLASH_ATTR ZFunctionClass_GetById( uint8_t id ) {
    int i = FindIndexByKey( &id, ARRAY_LEN( __FunctionClassesById ), &ZFunctionClass_CmpById );
    return ( ( i < 0 ) ? NULL : __FunctionClassesById[i] );
}
/*
//----------------------------------------------------------------------------

        static
        int
        ICACHE_FLASH_ATTR
        ZFunctionClass_CmpByName( const void* key, const int index )
        {
                return str_compare( (char*)key, __FunctionClassesByName[ index
]->_name );
        }

const ZFunctionClass
ICACHE_FLASH_ATTR
ZFunctionClass_GetByName( const char* name )
{
    int i = FindIndexByKey( name, ARRAY_LEN(__FunctionClassesByName),
&ZFunctionClass_CmpByName ); return ( (i < 0) ? NULL : __FunctionClassesByName[
i ] );
}
*/

//----------------------------------------------------------------------------
/*
const ZFunctionClass ICACHE_FLASH_ATTR
ZFunctionClass_GetSupportedById( const  uint8_t id )
{
    if( znet == NULL || id == 0 || !ZNet_IsFCSupported( znet, id ) )
        return NULL;
    const ZFunctionClass fc = ZFunctionClass_GetById( id );
    if( fc == NULL )
        fc = &fcDummyFunctionClass;
    return fc;
}
*/

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZFunctionClass_FillBySupportedFC( uint8_t* bitmask ) {
    clear_bitmask( bitmask );
    for( int i = 0; i < (int)ARRAY_LEN( __FunctionClassesById ); ++i ) {
        SET_BIT( bitmask, __FunctionClassesById[i]->_id );
    }
}

//////////////////////////////////////////////////////////////////////////////

#include "logger.h"

//----------------------------------------------------------------------------
ZFunction ICACHE_FLASH_ATTR ZFunction_Create( const ZFunctionClass const fc,
                                              void* initArg,
                                              ZFunction_Callback const Callback,
                                              void* callbackArg ) {
    ZFunction func = Heap_Alloc( sizeof( ZFunction_t ) );
    if( func ) {
        *( (ZNet*)&func->_znet ) = &znet_instance;
        *( (ZFunctionClass*)&func->_fc ) = (ZFunctionClass)fc;
        *( (ZFunction_Callback*)&func->_Callback ) = Callback;
        func->_callbackArg = callbackArg;
        func->_tx_frame = ( CyclicBuffer_t ){ ._data = NULL, ._size = 0, ._len = 0, ._begin = 0, ._end = 0 };
        func->_tx_frame_initial_len = 0;
        func->_timeout ^= func->_timeout;
        --func->_timeout;  // 0xFF...F
        func->_timeout_delta = 0;
        func->_ProcessRESPONSE = fc->_ProcessRESPONSE;
        func->_ProcessCALLBACK = fc->_ProcessCALLBACK;
        func->_security = Encapsulation_None;
        func->_endpoint = 0;
        func->_dataPtr = NULL;
        func->_dataSize = 0;
        func->_node_id = MY_NODE_ID;
        func->_tx_attempt_counter = 0;
        if( fc->_Init ) {
            if( ( *fc->_Init )( func, initArg ) ) {
                Heap_Free( func );
                Log_Write( "\r\nZFunction_Create Fail.\r\n" );
                return NULL;
            }
        }
    }
    return func;
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZFunction_Free( void* func ) {
    if( func == NULL )
        return;

    ZFunction this = (ZFunction)func;
    if( this->_dataPtr )
        Heap_Free( this->_dataPtr );
    if( this->_tx_frame._data )
        Heap_Free( this->_tx_frame._data );
    Heap_Free( this );
}

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckFuncToID( const void* object, const void* arg ) {
    ZFunction func = (ZFunction)object;
    uint8_t id = *(uint8_t*)arg;
    return ( id == func->_fc->_id );
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR CheckFuncToNodeID( const void* object, const void* arg ) {
    ZFunction func = (ZFunction)object;
    uint8_t node_id = *(uint8_t*)arg;
    return ( node_id == func->_node_id ) &&
           ( ( func->_fc->_id == fcSendData._id ) || ( func->_fc->_id == fcSendDataBridge._id ) );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZFunction_Free_From_All_List( const ZFunctionClass const fc ) {
    List_Entry entry;

    if( ( entry = List_Find( &znet_instance._funcs_to_TX, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "free function from _funcs_to_TX \r\n" );
        List_Remove( &znet_instance._funcs_to_TX, entry, ZFunction_Free );
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_ACK, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "free function from _funcs_to_ACK \r\n" );
        List_Remove( &znet_instance._funcs_to_ACK, entry, ZFunction_Free );
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_RESPONSE, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "free function from _funcs_to_RESPONSE \r\n" );
        List_Remove( &znet_instance._funcs_to_RESPONSE, entry, ZFunction_Free );
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_CALLBACK, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "free function from _funcs_to_CALLBACK \r\n" );
        List_Remove( &znet_instance._funcs_to_CALLBACK, entry, ZFunction_Free );
    }
}

//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR ZFunction_Find_Function_in_All_List( const ZFunctionClass const fc ) {
    List_Entry entry;

    if( ( entry = List_Find( &znet_instance._funcs_to_TX, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "find function in _funcs_to_TX \r\n" );
        return entry;
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_ACK, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "find function in _funcs_to_ACK \r\n" );
        return entry;
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_RESPONSE, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "find function in _funcs_to_RESPONSE \r\n" );
        return entry;
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_CALLBACK, &CheckFuncToID, &fc->_id ) ) ) {
        Log_Write( "find function in _funcs_to_CALLBACK \r\n" );
        return entry;
    }
    return NULL;
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR ZFunction_Check_is_Lists_Empty() {
    return List_IsEmpty( &znet_instance._funcs_to_ACK ) && List_IsEmpty( &znet_instance._funcs_to_RESPONSE ) &&
           List_IsEmpty( &znet_instance._funcs_to_CALLBACK );
}

//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR ZFunction_Find_Function_in_All_List_By_Init_Arg( List_CheckObjectFunc CheckFunc,
                                                                              const void* checkObjectFuncArg ) {
    List_Entry entry;
    if( ( entry = List_Find( &znet_instance._funcs_to_TX, CheckFunc, checkObjectFuncArg ) ) ) {
        Log_Write( "find function in _funcs_to_TX \r\n" );
        return entry;
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_ACK, CheckFunc, checkObjectFuncArg ) ) ) {
        Log_Write( "find function in _funcs_to_ACK \r\n" );
        return entry;
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_CALLBACK, CheckFunc, checkObjectFuncArg ) ) ) {
        Log_Write( "find function in _funcs_to_RESPONSE \r\n" );
        return entry;
    }
    if( ( entry = List_Find( &znet_instance._funcs_to_RESPONSE, CheckFunc, checkObjectFuncArg ) ) ) {
        Log_Write( "find function in _funcs_to_CALLBACK \r\n" );
        return entry;
    }
    return NULL;
}
