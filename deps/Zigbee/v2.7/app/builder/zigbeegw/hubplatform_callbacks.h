#pragma once
#include <stdint.h>

void hubPlatformAttributeNotificationCallbackUint8( uint16_t nodeId, uint8_t epId, uint16_t clsId,
        uint16_t attrId, uint8_t value ) __attribute__((weak));

void hubPlatformAttributeNotificationCallbackUint16( uint16_t nodeId, uint8_t epId, uint16_t clsId,
        uint16_t attrId, uint16_t value ) __attribute__((weak));

void hubPlatformIasZoneClusterZoneStatusChangeNotificationCallback(
        uint16_t nodeId, uint8_t epId, uint16_t clsId, uint16_t attrId,
        uint16_t zoneStatus, uint8_t extendedStatus, uint8_t zoneId, uint16_t delay )  __attribute__((weak));

void hubPlatformPollControlCheckInCallback( uint16_t nodeId, uint8_t epId, uint16_t clsId )  __attribute__((weak));
