#include <znet/znet_alarm_sensor.hpp>
#include <znet/znet_notification.hpp>

namespace znet {

namespace {
constexpr const char GeneralAlarmSensorName[] = "general";
}

AlarmSensorTypeOpt getAlarmSensorType( const char* sensorName ) {
    const auto extendedTypeOpt = getExtendedAlarmSensorType( sensorName );
    return extendedTypeOpt && *extendedTypeOpt < AlarmSensorType::FirstExtended ? extendedTypeOpt :
                                                                                  AlarmSensorTypeOpt{};
}

AlarmSensorTypeOpt getExtendedAlarmSensorType( const char* sensorName ) {
    const auto notifiactionTypeOpt = znet::getNotificationType( sensorName );
    return notifiactionTypeOpt ?
               static_cast<AlarmSensorType>( *notifiactionTypeOpt ) :
               strcmp( sensorName, GeneralAlarmSensorName ) == 0 ? AlarmSensorType::General : AlarmSensorTypeOpt{};
}

AlarmSensorTypeOpt getExtendedAlarmSensorType( const std::string& sensorName ) {
    return getExtendedAlarmSensorType( sensorName.c_str() );
}

const char* getAlarmSensorName( AlarmSensorType type ) {
    return type < AlarmSensorType::FirstExtended ? getExtendedAlarmSensorName( type ) : nullptr;
}

const char* getExtendedAlarmSensorName( AlarmSensorType type ) {
    return type == AlarmSensorType::General ? GeneralAlarmSensorName :
                                              getNotificationName( static_cast<NotificationType>( type ) );
}

}  // namespace znet
