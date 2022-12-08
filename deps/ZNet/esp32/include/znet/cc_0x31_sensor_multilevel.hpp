#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/enums/multilevel_sensor.hpp>

namespace znet {

class ZCc0x31SensorMultilevel final : public ZClass {
    struct SensorDescription {
        MultilevelSensor::Type type;  // Supported sensor
        uint8_t scalemask;            // Supported scales
    };

public:
    enum class Status { Idle, Polling };

    struct SensorInfo {
        MultilevelSensor::Type type;
        std::vector<uint8_t> scales;
    };

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL>;
    using ZClass::ZClass;

    ~ZCc0x31SensorMultilevel() override = default;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestValue( const RequestValue& request ) override;

    static bool isValidSensorType( MultilevelSensor::Type type );

    std::vector<SensorInfo> getSupportedSensorsInfo() const;

    ZNET_DEFINE_ACCEPT()

private:
    bool addSensorDescriptionImpl( MultilevelSensor::Type sensorType, uint8_t sensorMask );
    static std::vector<uint8_t> getSupportedScales( const SensorDescription& desc );

    bool requestNextComponent( InterviewCallbackPtr callback, WeakJobContext context, uint8_t index = 0 );

    void requestNextScale( InterviewCallbackPtr callback, WeakJobContext context, uint8_t index = 0 );

    void requestNextComponentInterview( InterviewCallbackPtr callback, WeakJobContext context, uint8_t index = 0 );

    bool addSensorDescription( MultilevelSensor::Type sensorType, uint8_t sensorScale );

    bool addSensorTypes( const uint8_t* data, uint8_t len );

private:
    static constexpr uint8_t ms_invalidScaleMask{ 0 };

    std::vector<SensorDescription> m_description;
    Status m_status = Status::Idle;
};

}  // namespace znet
