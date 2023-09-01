#pragma once

#include <znet/znet.h>
#include <znet/variant.hpp>
#include <znet/optional.hpp>

#include <cinttypes>
#include <vector>
#include <cstddef>
#include <string>
#include <string.h>
#include <algorithm>

namespace znet {

enum class NotificationType : uint8_t {
    SmokeAlarm = 0x01,
    COAlarm = 0x02,
    CO2Alarm = 0x03,
    HeatAlarm = 0x04,
    WaterAlarm = 0x05,
    AccessControl = 0x06,
    HomeSecurity = 0x07,
    PowerManagement = 0x08,
    System = 0x09,
    EmergencyAlarm = 0x0A,
    Clock = 0x0B,
    Appliance = 0x0C,
    HomeHealth = 0x0D,
    Siren = 0x0E,
    WaterValve = 0x0F,
    WeatherAlarm = 0x10,
    Irrigation = 0x11,
    GasAlarm = 0x12,
    PestControl = 0x13,
    LightSensor = 0x14,
    WaterQualityMonitoring = 0x15,
    HomeMonitoring = 0x16,
    RequestPendingNotification = 0xFF
};

using NotificationTypeOpt = utils::optional<NotificationType>;

using NotificationEvent = std::uint8_t;
using NotificationEventOpt = utils::optional<NotificationEvent>;

struct NotificationTimeValue {
    std::uint8_t m_hours;
    std::uint8_t m_minutes;
    std::uint8_t m_seconds;
};

struct Notification {
    NotificationType m_type;
    std::vector<NotificationEvent> m_events;
};

using NotificationValue = utils::variant<std::string, std::uint8_t, NotificationTimeValue>;
using NotificationValueOpt = utils::optional<NotificationValue>;

struct NotificationEventData {
    class ValueHandler {
    public:
        virtual ~ValueHandler() = default;
        virtual NotificationValueOpt getNotificationValue( const std::uint8_t* value, std::uint8_t size ) const = 0;
    };

    const char* m_name;
    znet_command_class_version_t m_version;
    const ValueHandler* m_valueHandler;
};

const NotificationEventData* getNotificationEventData( NotificationType notificationType, NotificationEvent eventType );
const char* getNotificationName( NotificationType notificationType );
NotificationTypeOpt getNotificationType( const char* notificationName );

inline NotificationTypeOpt getNotificationType( const std::string& notificationName ) {
    return getNotificationType( notificationName.c_str() );
}

NotificationEventOpt getNotificationEvent( NotificationType notificationType, const char* eventName );

inline NotificationEventOpt getNotificationEvent( NotificationType notificationType, const std::string& eventName ) {
    return getNotificationEvent( notificationType, eventName.c_str() );
}

}  // namespace znet
