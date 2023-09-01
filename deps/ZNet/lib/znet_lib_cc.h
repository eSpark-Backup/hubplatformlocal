
#ifndef __ZNET_LIB_CC_H__
#define __ZNET_LIB_CC_H__

#include <znet_lib_cc_application.h>
#include "znet.h"

#include "ZFunctionClass.h"

#include "znet_lib_cc_codes.h"

#include "znet_lib_cc_transport.h"
#include "znet_lib_cc_management.h"
#include "znet_lib_cc_network.h"

//////////////////////////////////////////////////////////////////////////////
#define MESSAGE_TYPE_UNKNOWN 0
#define MESSAGE_TYPE_REPORT 1
#define MESSAGE_TYPE_SET_GET 2
#define MESSAGE_TYPE_TRANSPORT 3

//----------------------------------------------------------------------------
typedef void ( *CC_Report )( const ZFunction func, uint8_t node_id, int len, const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
CC_Report ICACHE_FLASH_ATTR ZCommandClass_FindCCReportByCCData( const uint8_t* cc_report_data );

//----------------------------------------------------------------------------
uint8_t ICACHE_FLASH_ATTR ZCommandClass_FindTypeByCCData( const uint8_t* cc_report_data );

//----------------------------------------------------------------------------
extern void ICACHE_FLASH_ATTR ZNet_ApplicationCommandHandler( const uint8_t* buf,
                                                              uint8_t len,
                                                              uint8_t node_id,
                                                              encap_type_t security,
                                                              uint8_t endpoint );

//-----------------------------------------------------

#endif  // #ifndef __ZNET_LIB_CC_H__
