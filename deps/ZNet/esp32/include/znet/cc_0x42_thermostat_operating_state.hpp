#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

enum class ThermostatOperatingState;

class ZCc0x42ThermostatOpState final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

public:
    ThermostatOperatingState getOpStateLogging( uint8_t index ) const;
    size_t getOpStatesLoggingCount() const;

    ZNET_DEFINE_ACCEPT()

private:
    std::vector<ThermostatOperatingState> m_opstates;
};

}  // namespace znet
