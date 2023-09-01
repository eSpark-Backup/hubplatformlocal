#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x40ThermostatMode final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_THERMOSTAT_MODE>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    bool setManufacturerSpecificMode( const uint8_t* data, uint8_t size );

protected:
    bool setValue( const uint8_t value ) override;

public:
    uint8_t getMode( uint8_t index ) const;
    size_t getModesCount() const;
    void restoreMode( uint8_t mode );

    ZNET_DEFINE_ACCEPT()

private:
    std::vector<uint8_t> m_modes;
};

}  // namespace znet
