#ifndef __SERIAL_PORT_IMPL_H__
#define __SERIAL_PORT_IMPL_H__

//////////////////////////////////////////////////////////////////////////////

#include "buffers.h"
#include "serial_port_paremeters_types.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#ifdef SerialPortIdentifier_t
#undef SerialPortIdentifier_t
#endif
#ifdef SERIALPORT_RX_BUFFER_SIZE
#undef SERIALPORT_RX_BUFFER_SIZE
#endif
#ifdef SERIALPORT_TX_BUFFER_SIZE
#undef SERIALPORT_TX_BUFFER_SIZE
#endif
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/// INFO: for esp32 and linux use callback functions
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#define SerialPortIdentifier_t unsigned int
typedef struct _SerialPortImpl {
    int _isInit;
    int _isOpen;
} SerialPortImpl_t;
typedef SerialPortImpl_t* SerialPortImpl;
#else
#ifdef _WINDOWS
#include "serial_port_windows.h"
#else
/// INFO: for esp32 and linux use callback functions
#if( 0 && ( defined( __APPLE__ ) || defined( __linux__ ) ) )
#include "serial_port_linux.h"
#else
/// INFO: for esp32 and linux use callback functions
#if( 0 && defined( ESP_PLATFORM ) )
#include "serial_port_esp32.h"
#else
#include "serial_port_esp8266.h"
#endif
#endif
#endif
#endif

//////////////////////////////////////////////////////////////////////////////

bool ICACHE_FLASH_ATTR SerialPortImpl_Init( SerialPortImpl seriapPortImpl,
                                            SerialPortIdentifier_t serialPortId,
                                            Handshakings_e handshaking,
                                            BaudRates_t baudRate,
                                            DataBits_e dataBits,
                                            bool parityEnabled,
                                            Parities_e parity,
                                            StopBits_e stopBits );
void ICACHE_FLASH_ATTR SerialPortImpl_DeInit( SerialPortImpl serialPortImpl );

bool ICACHE_FLASH_ATTR SerialPortImpl_Open( SerialPortImpl serialPortImpl );
bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl serialPortImpl );
bool ICACHE_FLASH_ATTR SerialPortImpl_IsOpen( SerialPortImpl serialPortImpl );

int ICACHE_FLASH_ATTR SerialPortImpl_Read( SerialPortImpl serialPortImpl, int lenToRead, uint8_t* buffer );

int ICACHE_FLASH_ATTR SerialPortImpl_WriteCount( SerialPortImpl serialPortImpl );
int ICACHE_FLASH_ATTR SerialPortImpl_Write( SerialPortImpl serialPortImpl, int lenToWrite, uint8_t* buffer );

//////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR LogRawBuffer( const char* prefix, int orderedLen, int actualLen, uint8_t* buffer );

void ICACHE_FLASH_ATTR LogBuffer( const char* prefix, Buffer buff, int len );
void ICACHE_FLASH_ATTR LogCyclicBuffer( const char* prefix, CyclicBuffer buff, int len );
void ICACHE_FLASH_ATTR LogCyclicBuffer2( const char* prefix, const uint8_t type, CyclicBuffer buff, int len );

#endif  // #ifndef __SERIAL_PORT_IMPL_H__
