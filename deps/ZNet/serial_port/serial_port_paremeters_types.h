#ifndef __SERIAL_PORT_PARAMETERS_TYPES_H__
#define __SERIAL_PORT_PARAMETERS_TYPES_H__

//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
typedef enum {
    Handshakings_None = 0,
    Handshakings_XOnXoff = 1,
    Handshakings_RTS = 2,
    Handshakings_RTSXOnXoff = 3
} Handshakings_e;

//----------------------------------------------------------------------------
// 50,75,110,150,300,600,1200,1800,2000,2400,3600,4800,7200,9600,14400,19200,28800,38400,57600,115200,230400,250000,300000,375000,460800,500000,750000,921600,1500000,1843200,3686400
typedef uint32_t BaudRates_t;

//----------------------------------------------------------------------------
typedef enum { DataBits_5 = 5, DataBits_6 = 6, DataBits_7 = 7, DataBits_8 = 8 } DataBits_e;

//----------------------------------------------------------------------------
typedef enum {
    Parities_None = 0,
    Parities_Odd = 1,
    Parities_Even = 2,
    Parities_Mark = -1,
    Parities_Space = 32
} Parities_e;

//----------------------------------------------------------------------------
typedef enum {
    StopBits_1 = 2  // 1
    ,
    StopBits_15 = 3  // 1.5
    ,
    StopBits_2 = 4  // 2
} StopBits_e;

//////////////////////////////////////////////////////////////////////////////

#endif  // __SERIAL_PORT_PARAMETERS_TYPES_H__
