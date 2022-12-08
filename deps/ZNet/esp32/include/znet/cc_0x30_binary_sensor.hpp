#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x30BinarySensor final : public ZClass {
public:
    typedef enum {
        Reserved = 0x00,
        GeneralPurpose = 0x01,
        Smoke = 0x02,
        CO = 0x03,
        CO2 = 0x04,
        Heat = 0x05,
        Water = 0x06,
        Freeze = 0x07,
        Tamper = 0x08,
        Aux = 0x09,
        DoorWindow = 0x0A,
        Tilt = 0x0B,
        Motion = 0x0C,
        GlassBreak = 0x0D,
        Return1StSensorTypeOnSupportedList = 0xFF
    } SensorType;

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SENSOR_BINARY>;
    using ZClass::ZClass;

    ~ZCc0x30BinarySensor() override = default;
    std::vector<SensorType> getSensorTypes() const;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;
    bool requestValue( const RequestValue& request ) override;
    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

private:
    bool requestNextComponent( RequestValue::CallbackPtr callbackPtr, WeakJobContext jobContext, uint8_t index = 0 );

private:
    uint16_t m_sensorTypeMask = 0;
};

}  // namespace znet
