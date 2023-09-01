#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x44ThermostatFanMode final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

protected:
    bool setValue( const uint8_t value ) override;

public:
    uint8_t getFanMode( uint8_t index ) const;
    size_t getFanModesCount() const;
    void restoreFanMode( uint8_t mode );

    ZNET_DEFINE_ACCEPT()

private:
    std::vector<uint8_t> m_fanmodes;
};

}  // namespace znet
