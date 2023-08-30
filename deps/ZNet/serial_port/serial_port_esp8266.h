#ifndef __SERIAL_PORT_ESP8266_H__
#define __SERIAL_PORT_ESP8266_H__

#if( !defined( _WINDOWS ) && !defined( __linux__ ) && !defined( ESP_PLATFORM ) ) && !defined( __APPLE__ )

//////////////////////////////////////////////////////////////////////////////

#include "buffers.h"

#include "serial_port_paremeters_types.h"

//////////////////////////////////////////////////////////////////////////////

#define SerialPortIdentifier_t int
#define SERIALPORT_RX_BUFFER_SIZE 514
#define SERIALPORT_TX_BUFFER_SIZE 514

//----------------------------------------------------------------------------
typedef struct _SerialPortImpl {
    SerialPortIdentifier_t _serialPortId;

    int _device;

    bool _isOpen;

    BaudRates_t _baudRate;
    bool _parityEnabled;
    Parities_e _parity;
    DataBits_e _dataBits;
    StopBits_e _stopBits;
    Handshakings_e _handshaking;

    uint8_t _rxBufferData[SERIALPORT_RX_BUFFER_SIZE];
    CyclicBuffer_t _rxBuffer;

    uint8_t _txBufferData[SERIALPORT_TX_BUFFER_SIZE];
    CyclicBuffer_t _txBuffer;

} SerialPortImpl_t;
typedef SerialPortImpl_t* SerialPortImpl;

//////////////////////////////////////////////////////////////////////////////

#endif  // ( ! defined(_WINDOWS) && ! defined(__linux__) && !defined( ESP_PLATFORM ) )

#endif  // __SERIAL_PORT_ESP8266_H__
