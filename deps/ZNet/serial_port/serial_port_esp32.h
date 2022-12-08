#ifndef __SERIAL_PORT_ESP32_H__
#define __SERIAL_PORT_ESP32_H__

#if( defined( ESP_PLATFORM ) )

//////////////////////////////////////////////////////////////////////////////

#include "buffers.h"

#include "serial_port_paremeters_types.h"

//////////////////////////////////////////////////////////////////////////////

#define SerialPortIdentifier_t unsigned int

//----------------------------------------------------------------------------
typedef struct _SerialPortImpl {
    SerialPortIdentifier_t _serialPortId;

    bool _isInit;
    bool _isOpen;

    /*    BaudRates_t _baudRate;
        bool _parityEnabled;
        Parities_e _parity;
        DataBits_e _dataBits;
        StopBits_e _stopBits;
        Handshakings_e _handshaking;*/

} SerialPortImpl_t;
typedef SerialPortImpl_t* SerialPortImpl;

//////////////////////////////////////////////////////////////////////////////

#endif  // ( defined( ESP_PLATFORM ) )

#endif  // __SERIAL_PORT_ESP32_H__
