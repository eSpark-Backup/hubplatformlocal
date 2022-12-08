#include "znet/znet_meter.hpp"
#include "znet/znet_utils.hpp"
#include "znet/converters/resources.hpp"

namespace znet {
namespace {

using RangedStringList = utils::RangedList<std::uint8_t, const char*>;
using StringList = std::initializer_list<const char* const>;

struct MeterInfo {
    const char* name;
    RangedStringList scales;
};

using RangedMeterList = utils::RangedList<meter::MeterType, MeterInfo>;

const StringList ElectricScalesList_0 = {
    resources::scales::KiloWattHour,        // 0
    resources::scales::KiloVoltAmpereHour,  // 1
    resources::scales::Watt,                // 2
    resources::scales::PulseCount,          // 3
    resources::scales::Volt,                // 4
    resources::scales::Ampere,              // 5
    resources::scales::PowerFactor,         // 6
    nullptr,
    resources::scales::KiloVoltAmpereReactive,     // 8
    resources::scales::KiloVoltAmpereReactiveHour  // 9
};
const RangedStringList ElectricScalesList = { { 0, ElectricScalesList_0 } };

const StringList GasAndWaterScalesList_0 = {
    resources::scales::CubicMeter,  // 0
    resources::scales::CubicFeet,   // 1
    resources::scales::Gallons,     // 2
    resources::scales::PulseCount   // 3
};
const RangedStringList GasAndsWaterScalesList = { { 0, GasAndWaterScalesList_0 } };

const StringList HeatingAndCoolingScalesList_0 = {
    resources::scales::KiloWattHour,  // 0
};
const RangedStringList HeatingAndCoolingScalesList = { { 0, HeatingAndCoolingScalesList_0 } };

const std::initializer_list<const MeterInfo> MeterList_01 = {
    { "electric", ElectricScalesList },          // 0x01
    { "gas", GasAndsWaterScalesList },           // 0x02
    { "water", GasAndsWaterScalesList },         // 0x03
    { "heating", HeatingAndCoolingScalesList },  // 0x04
    { "cooling", HeatingAndCoolingScalesList },  // 0x05
};

const RangedMeterList MeterList = { { meter::MeterType::Electric, MeterList_01 } };

}  // namespace

namespace meter {

MeterTypeOpt stringToType( const char* name ) {
    return utils::findIndexInRangedList(
        MeterList, [name]( const MeterInfo& meterInfo ) { return strcmp( name, meterInfo.name ) == 0; } );
}

const char* typeToString( MeterType type ) {
    const auto meterInfo = utils::findInRangedList( MeterList, type );
    return meterInfo ? meterInfo->name : nullptr;
}

const char* scaleToString( MeterType type, uint8_t scale ) {
    const auto meterInfo = utils::findInRangedList( MeterList, type );
    if( !meterInfo )
        return nullptr;

    auto result = utils::findInRangedList( meterInfo->scales, scale );
    return result ? *result : nullptr;
}

ScaleOpt stringToScale( MeterType type, const char* scaleName ) {
    const auto meterInfo = utils::findInRangedList( MeterList, type );
    if( !meterInfo )
        return {};

    return utils::findIndexInRangedList( meterInfo->scales, [scaleName]( const char* value ) {
        return value != nullptr && strcmp( value, scaleName ) == 0;
    } );
}

bool isScaleSupported( MeterType type, uint8_t scale ) {
    return scaleToString( type, scale ) != nullptr;
}

}  // namespace meter
}  // namespace znet
