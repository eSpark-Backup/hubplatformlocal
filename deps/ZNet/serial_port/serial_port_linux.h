#ifndef __SERIAL_PORT_LINUX_H__
#define __SERIAL_PORT_LINUX_H__

#if( defined( __APPLE__ ) || defined( __linux__ ) )

//////////////////////////////////////////////////////////////////////////////

#include "buffers.h"

#include "serial_port_paremeters_types.h"

//////////////////////////////////////////////////////////////////////////////

#define SerialPortIdentifier_t const char*
//#define SERIALPORT_RX_BUFFER_SIZE  257
//#define SERIALPORT_TX_BUFFER_SIZE  257

//----------------------------------------------------------------------------
typedef struct _SerialPortImpl {
    SerialPortIdentifier_t _serialPortId;

    int _device;

    BaudRates_t _baudRate;
    bool _parityEnabled;
    Parities_e _parity;
    DataBits_e _dataBits;
    StopBits_e _stopBits;
    Handshakings_e _handshaking;

    // uint8_t                _rxBufferData[ SERIALPORT_RX_BUFFER_SIZE ];
    // CyclicBuffer_t         _rxBuffer;

    // uint8_t                _txBufferData[ SERIALPORT_TX_BUFFER_SIZE ];
    // CyclicBuffer_t         _txBuffer;

    // Activity_t             _rxActivity;
    // Activity_t             _txActivity;

} SerialPortImpl_t;
typedef SerialPortImpl_t* SerialPortImpl;

//////////////////////////////////////////////////////////////////////////////

#endif  // __linux__

#endif  // __SERIAL_PORT_LINUX_H__
