#include <znet/converters/thermostat_operating_state_converter.hpp>

#include <znet/enums/thermostat_operating_state.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

#include <cassert>

namespace znet {
namespace thermostat_operating_state_converter {

using namespace resources::cc_thermostat_operating_state;

namespace {

struct ThermostatOperatingStateData {
    const ThermostatOperatingState scale;
    const char* const name;
};

const ThermostatOperatingStateData thermostatOperatingStateData[] = {
    { ThermostatOperatingState::Idle, type::Idle },
    { ThermostatOperatingState::Heating, type::Heating },
    { ThermostatOperatingState::Cooling, type::Cooling },
    { ThermostatOperatingState::FanOnly, type::FanOnly },
    { ThermostatOperatingState::PendingHeat, type::PendingHeat },
    { ThermostatOperatingState::PendingCool, type::PendingCool },
    { ThermostatOperatingState::VentEco, type::VentEco },
    { ThermostatOperatingState::AuxHeating, type::AuxHeating },
    { ThermostatOperatingState::SecStageHeating, type::SecStageHeating },
    { ThermostatOperatingState::SecStageCooling, type::SecStageCooling },
    { ThermostatOperatingState::SecStageAuxHeat, type::SecStageAuxHeat },
    { ThermostatOperatingState::ThirdStageAuxHeat, type::ThirdStageAuxHeat }
};

static_assert( static_cast<std::size_t>( ThermostatOperatingState::Count ) ==
                   utils::arraySize( thermostatOperatingStateData ),
               "Data for new thermostat operating state type is missing" );

const ThermostatOperatingStateData* findThermostatOperatingStateData( ThermostatOperatingState scale ) {
    for( const auto& data : thermostatOperatingStateData ) {
        if( data.scale == scale ) {
            return &data;
        }
    }

    assert( !"Unsupported ThermostatOperatingState" );

    return nullptr;
}

}  // namespace

const char* typeToString( ThermostatOperatingState scale ) noexcept {
    auto date = findThermostatOperatingStateData( scale );
    return date ? date->name : nullptr;
}

}  // namespace thermostat_operating_state_converter
}  // namespace znet
