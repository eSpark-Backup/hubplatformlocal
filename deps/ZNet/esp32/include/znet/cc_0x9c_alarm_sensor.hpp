#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_alarm_sensor.hpp>

#include <vector>

class IFile;

namespace znet {

class ZCc0x9cAlarmSensor final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SENSOR_ALARM>;
    using ZClass::ZClass;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;
    bool requestValue( const RequestValue& request ) override;
    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    std::vector<AlarmSensorType> getSupportedTypes() const;

    ZNET_DEFINE_ACCEPT()

private:
    bool requestNextComponent( RequestValue::CallbackPtr callbackPtr, WeakJobContext jobContext, uint8_t index = 0 );

private:
    uint8_t m_sensorTypeMask = 0;
};

}  // namespace znet
