#pragma once

#include <znet/znet_utils.hpp>
#include <string>

namespace znet {

/* @brief Alarm sensor types with extension.
 * Alarm sensor cc uses only range [General; WaterAlarm]
 * Notification cc is extension of Alarm sensor cc.
 * Notification cc uses range [SmokeAlarm; WaterAlarm] and some types from range [FirstExtended; LastExtended]. See
 * NotificationType for details. Alarm silence cc can potentially use all range of alarm sensor types,
 */
enum class AlarmSensorType : uint8_t {
    General = 0x00,

    SmokeAlarm = 0x01,
    COAlarm = 0x02,
    CO2Alarm = 0x03,
    HeatAlarm = 0x04,
    WaterAlarm = 0x05,

    FirstExtended,
    LastExtended = 0xFF
};

using AlarmSensorTypeOpt = utils::optional<AlarmSensorType>;

AlarmSensorTypeOpt getAlarmSensorType( const char* sensorName );
AlarmSensorTypeOpt getExtendedAlarmSensorType( const char* sensorName );
AlarmSensorTypeOpt getExtendedAlarmSensorType( const std::string& sensorName );

const char* getAlarmSensorName( AlarmSensorType type );
const char* getExtendedAlarmSensorName( AlarmSensorType type );

struct ExtendedAlarmSensorTypesMask {
    uint8_t m_mask[256 / 8];
    uint8_t m_length;
};

template <typename Iterator, typename UnknownSensorHandler>
ExtendedAlarmSensorTypesMask getExtendedAlarmSensorTypesMask( Iterator rangeBegin,
                                                              Iterator rangeEnd,
                                                              UnknownSensorHandler unknownSensorHandler ) {
    ExtendedAlarmSensorTypesMask result;
    result.m_length =
        utils::writeBitmaskByRange( rangeBegin,
                                    rangeEnd,
                                    result.m_mask,
                                    sizeof( result.m_mask ),
                                    []( decltype( *rangeBegin ) value ) { return getExtendedAlarmSensorType( value ); },
                                    std::move( unknownSensorHandler ) );

    return result;
}

}  // namespace znet
