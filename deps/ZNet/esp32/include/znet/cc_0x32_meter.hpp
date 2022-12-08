#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_utils.hpp>
#include <znet/enums/meter_type.hpp>

namespace znet {

class ZCc0x32Meter final : public ZClass {
    enum class Status { Idle, Polling };

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_METER>;

    ZCc0x32Meter( ZChannel& parent, znet_command_class_version_t version = 1 );

    template <typename Value,
              typename std::enable_if<utils::HasType<Value, int8_t, int16_t, int32_t>::value>::type* = nullptr>
    bool resetMeter( Value value ) {
        return resetMeter( sizeof( value ), reinterpret_cast<const uint8_t*>( &value ) );
    }

    bool resetMeter( uint8_t dataSize, const uint8_t* data );

    bool resetMeter();

    bool requestMeterComponent( uint8_t scale );

    bool requestValue( const RequestValue& request ) override;
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    virtual bool storeData( IFile& file ) const override;
    virtual bool restoreData( IFile& file, int version ) override;

public:
    void setResetSupported( bool reset );
    bool isResetSupported() const;

    meter::MeterType getType() const;

    meter::Scale getScale( uint8_t index ) const;

    size_t getScaleCount() const;

    bool hasScale( meter::Scale scale ) const;

    const std::vector<uint8_t>& getScales() const;

    ZNET_DEFINE_ACCEPT()
protected:
    bool setValue( const uint8_t value ) override;

private:
    bool requestNextComponent( RequestValue::CallbackPtr callback, WeakJobContext jobContext, uint8_t index = 0 );

private:
    bool m_resetSupported = false;
    meter::MeterType m_type = meter::MeterType::Unknown;
    std::vector<uint8_t> m_scales;
    Status m_status;
};

}  // namespace znet
