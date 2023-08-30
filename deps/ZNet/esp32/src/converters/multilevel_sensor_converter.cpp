#include <znet/converters/multilevel_sensor_converter.hpp>

#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

#include <cassert>

namespace znet {
namespace multilevel_sensor_converter {

using namespace resources;
using namespace cc_sensor_multilevel;

struct MultilevelSensorData {
    MultilevelSensor::Type sensorType;
    const char* name;
    const char* scales[MultilevelSensor::MAX_SCALES];
};

static const MultilevelSensorData multilevelSensorData[] = {
    { MultilevelSensor::Reserved, nullptr, {} },
    { MultilevelSensor::AirTemperature, sensor_types::AirTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::GeneralPurpose, sensor_types::GeneralPurpose, { scales::Percent, scales::Float } },
    { MultilevelSensor::Illuminance, sensor_types::Illuminance, { scales::Percent, scales::Lux } },
    { MultilevelSensor::Power, sensor_types::Power, { scales::Watt, scales::BtuPerHour } },
    { MultilevelSensor::Humidity, sensor_types::Humidity, { scales::Percent, scales::GramPerCubicMeter } },
    { MultilevelSensor::Velocity, sensor_types::Velocity, { scales::MeterPerSecond, scales::MilePerHour } },
    { MultilevelSensor::Direction, sensor_types::Direction, { scales::Direction } },
    { MultilevelSensor::AtmosphericPressure,
      sensor_types::AtmosphericPressure,
      { scales::KiloPascal, scales::InchesOfMercury } },
    { MultilevelSensor::BarometricPressure,
      sensor_types::BarometricPressure,
      { scales::KiloPascal, scales::InchesOfMercury } },
    { MultilevelSensor::SolarRadiation, sensor_types::SolarRadiation, scales::WattPerSquareMeter },
    { MultilevelSensor::DewPoint, sensor_types::DewPoint, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::RainRate, sensor_types::RainRate, { scales::MilliMeterPerHour, scales::InchesPerHour } },
    { MultilevelSensor::TideLevel, sensor_types::TideLevel, { scales::Meter, scales::Feet } },
    { MultilevelSensor::Weight, sensor_types::Weight, { scales::KiloGram, scales::Pounds } },
    { MultilevelSensor::Voltage, sensor_types::Voltage, { scales::Volt, scales::MilliVolt } },
    { MultilevelSensor::Current, sensor_types::Current, { scales::Ampere, scales::MilliAmpere } },
    { MultilevelSensor::CO2Level, sensor_types::CO2Level, { scales::PartsPerMillion } },
    { MultilevelSensor::AirFlow, sensor_types::AirFlow, { scales::CubicMeterPerHour, scales::CubicFeetPerMinute } },
    { MultilevelSensor::TankCapacity,
      sensor_types::TankCapacity,
      { scales::Liter, scales::CubicMeter, scales::Gallons } },
    { MultilevelSensor::Distance, sensor_types::Distance, { scales::Meter, scales::CentiMeter, scales::Feet } },
    { MultilevelSensor::AnglePosition,
      sensor_types::AnglePosition,
      { scales::Percent, scales::NorthPoleDegress, scales::NorthPoleDegress } },
    { MultilevelSensor::Rotation, sensor_types::Rotation, { scales::RevolutionsPerMinute, scales::Hertz } },
    { MultilevelSensor::WaterTemperature, sensor_types::WaterTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::SoilTemperature, sensor_types::SoilTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::SeismicIntensity,
      sensor_types::SeismicIntensity,
      { scales::Mercalli, scales::EuropeanMacroseismic, scales::Liedu, scales::Shindo } },
    { MultilevelSensor::SeismicMagnitude,
      sensor_types::SeismicMagnitude,
      { scales::Local, scales::Moment, scales::SurfaceWave, scales::BodyWave } },
    { MultilevelSensor::Ultraviolet, sensor_types::Ultraviolet, { scales::UVIndex } },
    { MultilevelSensor::ElectricalResistivity, sensor_types::ElectricalResistivity, { scales::OhmMeter } },
    { MultilevelSensor::ElectricalConductivity, sensor_types::ElectricalConductivity, { scales::SiemensPerMeter } },
    { MultilevelSensor::Loudness, sensor_types::Loudness, { scales::Decibel, scales::AWeightedDecibels } },
    { MultilevelSensor::Moisture,
      sensor_types::Moisture,
      { scales::Percent, scales::VolumeWaterContent, scales::Impedance, scales::WaterActivity } },
    { MultilevelSensor::Frequency, sensor_types::Frequency, { scales::Hertz, scales::KiloHertz } },
    { MultilevelSensor::Time, sensor_types::Time, { scales::Seconds } },
    { MultilevelSensor::TargetTemperature, sensor_types::TargetTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::ParticulateMatter_2_5,
      sensor_types::ParticulateMatter_2_5,
      { scales::MolePerCubicMeter, scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::FormaldehydeLevel, sensor_types::FormaldehydeLevel, { scales::MolePerCubicMeter } },
    { MultilevelSensor::RadonConcentration,
      sensor_types::RadonConcentration,
      { scales::BecquerelPerCubicMeter, scales::PicocuriesPerLiter } },
    { MultilevelSensor::MethaneDensity, sensor_types::MethaneDensity, { scales::MolePerCubicMeter } },
    { MultilevelSensor::VolatileOrganicCompoundLevel,
      sensor_types::VolatileOrganicCompoundLevel,
      { scales::MolePerCubicMeter, scales::PartsPerMillion } },
    { MultilevelSensor::CarbonMonoxideLevel,
      sensor_types::CarbonMonoxideLevel,
      { scales::MolePerCubicMeter, scales::PartsPerMillion } },
    { MultilevelSensor::SoilHumidity, sensor_types::SoilHumidity, { scales::Percent } },
    { MultilevelSensor::SoilReactivity, sensor_types::SoilReactivity, { scales::PotentialOfHydrogen } },
    { MultilevelSensor::SoilSalinity, sensor_types::SoilSalinity, { scales::MolePerCubicMeter } },
    { MultilevelSensor::HeartRate, sensor_types::HeartRate, { scales::BeatsPerMinute } },
    { MultilevelSensor::BloodPressure, sensor_types::BloodPressure, { scales::Systolic, scales::Diastolic } },
    { MultilevelSensor::MuscleMass, sensor_types::MuscleMass, { scales::KiloGram } },
    { MultilevelSensor::FatMass, sensor_types::FatMass, { scales::KiloGram } },
    { MultilevelSensor::BoneMass, sensor_types::BoneMass, { scales::KiloGram } },
    { MultilevelSensor::TotalBodyWater, sensor_types::TotalBodyWater, { scales::KiloGram } },
    { MultilevelSensor::BasisMetabolicRate, sensor_types::BasisMetabolicRate, { scales::Joule } },
    { MultilevelSensor::BodyMassIndex, sensor_types::BodyMassIndex, { scales::Float } },
    { MultilevelSensor::AccelerationXAxis, sensor_types::AccelerationXAxis, { scales::MeterPerSquareSecond } },
    { MultilevelSensor::AccelerationYAxis, sensor_types::AccelerationYAxis, { scales::MeterPerSquareSecond } },
    { MultilevelSensor::AccelerationZAxis, sensor_types::AccelerationZAxis, { scales::MeterPerSquareSecond } },
    { MultilevelSensor::SmokeDensity, sensor_types::SmokeDensity, { scales::Percent } },
    { MultilevelSensor::WaterFlow, sensor_types::WaterFlow, { scales::LiterPerHour } },
    { MultilevelSensor::WaterPressure, sensor_types::WaterPressure, { scales::KiloPascal } },
    { MultilevelSensor::RFSignalStrength,
      sensor_types::RFSignalStrength,
      { scales::Percent, scales::DecibelMilliWatt } },
    { MultilevelSensor::ParticulateMatter_10,
      sensor_types::ParticulateMatter_10,
      { scales::MolePerCubicMeter, scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::RespiratoryRate, sensor_types::RespiratoryRate, { scales::BreathsPerMinute } },
    { MultilevelSensor::RelativeModulationLevel, sensor_types::RelativeModulationLevel, { scales::Percent } },
    { MultilevelSensor::BoilerWaterTemperature,
      sensor_types::BoilerWaterTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::DomesticHotWaterTemperature,
      sensor_types::DomesticHotWaterTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::OutsideTemperature, sensor_types::OutsideTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::ExhaustTemperature, sensor_types::ExhaustTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::WaterChlorineLevel, sensor_types::WaterChlorineLevel, { scales::MilliGramPerLiter } },
    { MultilevelSensor::WaterAcidity, sensor_types::WaterAcidity, { scales::PotentialOfHydrogen } },
    { MultilevelSensor::WaterOxidationReductionPotential,
      sensor_types::WaterOxidationReductionPotential,
      { scales::MilliVolt } },
    { MultilevelSensor::HeartRate_LFHF_Ratio, sensor_types::HeartRate_LFHF_Ratio, { scales::Float } },
    { MultilevelSensor::MotionDirection, sensor_types::MotionDirection, { scales::Direction } },
    { MultilevelSensor::AppliedForceOnSensor, sensor_types::AppliedForceOnSensor, { scales::Newton } },
    { MultilevelSensor::ReturnAirTemperature,
      sensor_types::ReturnAirTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::SupplyAirTemperature,
      sensor_types::SupplyAirTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::CondenserCoilTemperature,
      sensor_types::CondenserCoilTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::EvaporatorCoilTemperature,
      sensor_types::EvaporatorCoilTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::LiquidLineTemperature,
      sensor_types::LiquidLineTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::DischargeLineTemperature,
      sensor_types::DischargeLineTemperature,
      { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::SuctionPressure,
      sensor_types::SuctionPressure,
      { scales::KiloPascal, scales::PoundPerSquareInch } },
    { MultilevelSensor::DischargePressure,
      sensor_types::DischargePressure,
      { scales::KiloPascal, scales::PoundPerSquareInch } },
    { MultilevelSensor::DefrostTemperature, sensor_types::DefrostTemperature, { scales::Celsius, scales::Fahrenheit } },
    { MultilevelSensor::Ozone, sensor_types::Ozone, { scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::SulfurDioxide, sensor_types::SulfurDioxide, { scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::NitrogenDioxide, sensor_types::NitrogenDioxide, { scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::Ammonia, sensor_types::Ammonia, { scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::Lead, sensor_types::Lead, { scales::MicroGramPerCubicMeter } },
    { MultilevelSensor::ParticulateMatter_1, sensor_types::ParticulateMatter_1, { scales::MicroGramPerCubicMeter } }
};

static_assert( MultilevelSensor::Count == utils::arraySize( multilevelSensorData ),
               "Data for new multilevel sensor type is missing" );

static const MultilevelSensorData* findSensorData( MultilevelSensor::Type sensorType ) {
    for( const auto& data : multilevelSensorData ) {
        if( data.sensorType == sensorType ) {
            return &data;
        }
    }

    assert( false );

    return nullptr;
}

const char* typeToString( MultilevelSensor::Type sensorType ) {
    auto sensorData = findSensorData( sensorType );
    return sensorData ? sensorData->name : nullptr;
}
const char* scaleToString( MultilevelSensor::Type sensorType, uint8_t sensorScale ) {
    if( sensorScale > MultilevelSensor::MAX_SCALES ) {
        assert( false );
        return nullptr;
    }

    auto sensorData = findSensorData( sensorType );
    return sensorData ? sensorData->scales[sensorScale] : nullptr;
}

}  // namespace multilevel_sensor_converter
}  // namespace znet
