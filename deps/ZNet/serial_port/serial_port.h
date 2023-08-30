#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

//////////////////////////////////////////////////////////////////////////////

#include "serial_port_paremeters_types.h"
#include "serial_port_impl.h"

//////////////////////////////////////////////////////////////////////////////

typedef struct _SerialPort {
    SerialPortImpl_t _impl;

} SerialPort_t;
typedef SerialPort_t* SerialPort;

//////////////////////////////////////////////////////////////////////////////

bool ICACHE_FLASH_ATTR SerialPort_Init( SerialPort serialPort,
                                        SerialPortIdentifier_t serialPortId,
                                        Handshakings_e handshaking,
                                        BaudRates_t baudRate,
                                        DataBits_e dataBits,
                                        bool parityEnabled,
                                        Parities_e parity,
                                        StopBits_e stopBits );

void ICACHE_FLASH_ATTR SerialPort_DeInit( SerialPort serialPort );

bool ICACHE_FLASH_ATTR SerialPort_Open( SerialPort serialPort );
bool ICACHE_FLASH_ATTR SerialPort_Close( SerialPort serialPort );
bool ICACHE_FLASH_ATTR SerialPort_ReOpen( SerialPort serialPort );

int ICACHE_FLASH_ATTR SerialPort_ReadCyclicBuffer( SerialPort serialPort, CyclicBuffer buffer );

int ICACHE_FLASH_ATTR SerialPort_WriteMem( SerialPort serialPort, int lenToWrite, uint8_t* buffer );
int ICACHE_FLASH_ATTR SerialPort_WriteBuffer( SerialPort serialPort, int lenToWrite, Buffer buffer );
int ICACHE_FLASH_ATTR SerialPort_WriteCyclicBuffer( SerialPort serialPort, int lenToWrite, CyclicBuffer buffer );

//////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __SERIAL_PORT_H__
