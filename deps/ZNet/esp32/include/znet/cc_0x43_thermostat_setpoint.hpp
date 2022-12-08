#pragma once

#include <znet/znet_command_class.hpp>

#include <functional>

#define ZNET_THERMOSTAT_SETPOINT_VER3_INC_VALUE 0x02
#define ZNET_THERMOSTAT_SETPOINT_VER3_NO_INC_VALUE 0x07
#define ZNET_THERMOSTAT_SETPOINT_VER3_CORRECTOR 0x04

namespace znet {

class ZCc0x43ThermostatSetpoint final : public ZClass {
public:
    struct setpointsDescription_t {
        setpointsDescription_t( uint8_t type, float minValue, uint8_t minScale, float maxValue, uint8_t maxScale )
        : type{ type }
        , minValue{ minValue }
        , minScale{ minScale }
        , maxValue{ maxValue }
        , maxScale{ maxScale } {}

        setpointsDescription_t() = default;

        uint8_t type = 0;
        float minValue = 0;
        uint8_t minScale = 1;
        float maxValue = 212;  // 212 °F is boiling point
        uint8_t maxScale = 1;
    };

    struct setpointsDescription_t_v5 : setpointsDescription_t {
        setpointsDescription_t_v5( uint8_t type, float minValue, uint8_t minScale, float maxValue, uint8_t maxScale )
        : setpointsDescription_t( type, minValue, minScale, maxValue, maxScale )
        , size{ 0 }
        , precision{ 0 } {}

        setpointsDescription_t_v5( const setpointsDescription_t& sp )
        : setpointsDescription_t( sp.type, sp.minValue, sp.minScale, sp.maxValue, sp.maxScale )
        , size{ 0 }
        , precision{ 0 } {}

        setpointsDescription_t_v5() = default;

        uint8_t size = 0;
        uint8_t precision = 0;
    };

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT>;
    using ZClass::ZClass;

    ~ZCc0x43ThermostatSetpoint() override = default;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestValue( const RequestValue& request ) override;

    bool setValue( uint8_t setpointType, uint8_t scale, float value );

    uint8_t getSize( uint8_t type );

    uint8_t getPrecision( uint8_t type );

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    const std::vector<setpointsDescription_t_v5>& getDescription() const;

    bool requestSetpoint(
        const uint8_t type,
        std::function<void( const znet_thermostat_setpoint_report_t&, bool& requested )> onSuccess = nullptr,
        std::function<void( RequestFailType )> onFail = nullptr );

    ZNET_DEFINE_ACCEPT()

private:
    uint8_t getScale( uint8_t params ) const;

    float getConvertedValue( uint8_t params, int32_t value ) const;

    void setSetpointTypes( const uint8_t* data, uint8_t len );

    void requestNextSetpointCapability( RequestValue::CallbackPtr callbackPtr,
                                        WeakJobContext context,
                                        uint8_t index = 0 );

    void requestNextSetpointInterview( RequestValue::CallbackPtr callbackPtr,
                                       WeakJobContext context,
                                       uint8_t index = 0 );

    bool requestNextSetpoint( InterviewCallbackPtr callbackPtr, WeakJobContext context, uint8_t index = 0 );

    bool isValidSetpointType( uint8_t type ) const;

private:
    size_t m_currentCapabilitiesIndexAtInterview = 0;
    std::vector<setpointsDescription_t_v5> m_description;
};

}  // namespace znet
