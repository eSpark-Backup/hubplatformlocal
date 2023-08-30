
#ifndef __ZNET_ACTIVITIES_H__
#define __ZNET_ACTIVITIES_H__

#include "ZErrors.h"

#include "znet.h"
#include "ZFunctionClass.h"

void ICACHE_FLASH_ATTR ZNetRXActivity_init();
uint8_t ZNetRXActivityFunc();

FuncResults_e ICACHE_FLASH_ATTR DefaultFunctionClassAck( ZNet znet );
FuncResults_e ICACHE_FLASH_ATTR DefaultFunctionClassResponse( int size, const uint8_t* data );
FuncResults_e ICACHE_FLASH_ATTR DefaultFunctionClassCallback( int size, const uint8_t* data );

#endif  // #ifndef __ZNET_ACTIVITIES_H__
