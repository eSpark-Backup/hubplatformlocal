#pragma once

namespace znet {
namespace resources {

namespace err {
extern const char Channel[];
extern const char CommandClass[];
extern const char DataLength[];
extern const char FailType[];
extern const char NodeId[];
extern const char Offset[];
extern const char PreviousError[];
extern const char ReadBytes[];
extern const char Reason[];
extern const char ReportedOffset[];
extern const char RequestedOffset[];
extern const char RequestName[];
extern const char Status[];
extern const char Step[];
}  // namespace err

namespace modes {

constexpr char Initializing[] = "initializing";
constexpr char IncludeStarting[] = "include_starting";
constexpr char IncludeStopping[] = "include_stopping";
constexpr char ExcludeStarting[] = "exclude_starting";
constexpr char ExcludeStopping[] = "exclude_stopping";
constexpr char LearnStarting[] = "learn_starting";
constexpr char LearnStopping[] = "learn_stopping";
constexpr char Monitoring[] = "monitoring";
constexpr char NifSending[] = "nif_sending";
constexpr char NodeAdding[] = "node_adding";
constexpr char NodeRemoving[] = "node_removing";
constexpr char NodeLearning[] = "node_learning";
constexpr char NodeListGetting[] = "node_list_getting";
constexpr char ValuesGetting[] = "values_getting";
constexpr char Reseting[] = "reseting";
constexpr char Idle[] = "idle";
constexpr char TimeSynchronization[] = "time_synchronization";
constexpr char Rediscover[] = "rediscover";
constexpr char FirmwareUpdate[] = "firmware_update";
constexpr char NodeChecking[] = "node_checking";
constexpr char Unknown[] = "unknown";

}  // namespace modes

namespace mode_statuses {

constexpr char Invoked[] = "invoked";
constexpr char Started[] = "started";
constexpr char Finished[] = "finished";
constexpr char InProgress[] = "in_progress";
constexpr char Failed[] = "failed";
constexpr char Timeout[] = "timeout";
constexpr char Busy[] = "busy";
constexpr char Empty[] = "empty";
constexpr char Undefined[] = "undefined";
constexpr char Unknown[] = "Unknown";

}  // namespace mode_statuses

namespace scales {

constexpr char Percent[] = "percent";
constexpr char Lux[] = "lux";
constexpr char KiloPascal[] = "kilo_pascal";
constexpr char PoundPerSquareInch[] = "pound_per_square_inch";
constexpr char InchesOfMercury[] = "inches_of_mercury";
constexpr char GramPerCubicMeter[] = "gram_per_cubic_meter";
constexpr char MicroGramPerCubicMeter[] = "micro_gram_per_cubic_meter";
constexpr char MolePerCubicMeter[] = "mole_per_cubic_meter";
constexpr char PartsPerMillion[] = "parts_per_million";
constexpr char MilliGramPerLiter[] = "milli_gram_per_liter";
constexpr char Watt[] = "watt";
constexpr char BtuPerHour[] = "btu_per_hour";
constexpr char MeterPerSecond[] = "meter_per_second";
constexpr char MilePerHour[] = "mile_per_hour";
constexpr char MeterPerSquareSecond[] = "meter_per_square_second";
constexpr char Direction[] = "direction";
constexpr char Float[] = "float";
constexpr char PotentialOfHydrogen[] = "potential_of_hydrogen";
constexpr char MilliVolt[] = "milli_volt";
constexpr char Volt[] = "volt";
constexpr char Ampere[] = "ampere";
constexpr char MilliAmpere[] = "milli_ampere";
constexpr char Newton[] = "newton";
constexpr char WattPerSquareMeter[] = "watt_per_square_meter";
constexpr char MilliMeterPerHour[] = "milli_meter_per_hour";
constexpr char InchesPerHour[] = "inches_per_hour";
constexpr char Meter[] = "meter";
constexpr char Feet[] = "feet";
constexpr char CentiMeter[] = "centi_meter";
constexpr char KiloGram[] = "kilo_gram";
constexpr char Pounds[] = "pounds";
constexpr char CubicMeterPerHour[] = "cubic_meter_per_hour";
constexpr char CubicFeetPerMinute[] = "cubic_feet_per_minute";
constexpr char LiterPerHour[] = "liter_per_hour";
constexpr char Liter[] = "liter";
constexpr char CubicMeter[] = "cubic_meter";
constexpr char CubicFeet[] = "cubic_feet";
constexpr char Gallons[] = "gallons";
constexpr char NorthPoleDegress[] = "north_pole_degress";
constexpr char RevolutionsPerMinute[] = "revolutions_per_minute";
constexpr char BeatsPerMinute[] = "beats_per_minute";
constexpr char BreathsPerMinute[] = "breaths_per_minute";
constexpr char Hertz[] = "hertz";
constexpr char KiloHertz[] = "kilo_hertz";
constexpr char Mercalli[] = "mercalli";
constexpr char EuropeanMacroseismic[] = "european_macroseismic";
constexpr char Liedu[] = "liedu";
constexpr char Shindo[] = "shindo";
constexpr char Local[] = "local";
constexpr char Moment[] = "moment";
constexpr char SurfaceWave[] = "surface_wave";
constexpr char BodyWave[] = "body_wave";
constexpr char UVIndex[] = "uv_index";
constexpr char OhmMeter[] = "ohm_meter";
constexpr char SiemensPerMeter[] = "siemens_per_meter";
constexpr char Decibel[] = "decibel";
constexpr char AWeightedDecibels[] = "a_weighted_decibels";
constexpr char VolumeWaterContent[] = "volume_water_content";
constexpr char Impedance[] = "impedance";
constexpr char WaterActivity[] = "water_activity";
constexpr char Seconds[] = "seconds";
constexpr char BecquerelPerCubicMeter[] = "becquerel_per_cubic_meter";
constexpr char PicocuriesPerLiter[] = "picocuries_per_liter";
constexpr char Systolic[] = "systolic";
constexpr char Diastolic[] = "diastolic";
constexpr char Joule[] = "joule";
constexpr char DecibelMilliWatt[] = "decibel_milli_watt";
constexpr char Celsius[] = "celsius";
constexpr char Fahrenheit[] = "fahrenheit";

constexpr char PowerFactor[] = "power_factor";
constexpr char PulseCount[] = "pulse_count";
constexpr char KiloWattHour[] = "kilo_watt_hour";
constexpr char KiloVoltAmpereHour[] = "kilo_volt_ampere_hour";
constexpr char KiloVoltAmpereReactive[] = "kilo_volt_ampere_reactive";
constexpr char KiloVoltAmpereReactiveHour[] = "kilo_volt_ampere_reactive_hour";
}  // namespace scales

namespace cc_sensor_multilevel {

constexpr char Name[] = "cc_0x31_sensor_multilevel";

namespace sensor_types {
constexpr char AirTemperature[] = "air_temperature";
constexpr char GeneralPurpose[] = "general_purpose";
constexpr char Illuminance[] = "illuminance";
constexpr char Power[] = "power";
constexpr char Humidity[] = "humidity";
constexpr char Velocity[] = "velocity";
constexpr char Direction[] = "direction";
constexpr char AtmosphericPressure[] = "atmospheric_pressure";
constexpr char BarometricPressure[] = "barometric_pressure";
constexpr char SolarRadiation[] = "solar_radiation";
constexpr char DewPoint[] = "dew_point";
constexpr char RainRate[] = "rain_rate";
constexpr char TideLevel[] = "tide_level";
constexpr char Weight[] = "weight";
constexpr char Voltage[] = "voltage";
constexpr char Current[] = "current";
constexpr char CO2Level[] = "co2_level";
constexpr char AirFlow[] = "air_flow";
constexpr char TankCapacity[] = "tank_capacity";
constexpr char Distance[] = "distance";
constexpr char AnglePosition[] = "angle_position";
constexpr char Rotation[] = "rotation";
constexpr char WaterTemperature[] = "water_temperature";
constexpr char SoilTemperature[] = "soil_temperature";
constexpr char SeismicIntensity[] = "seismic_intensity";
constexpr char SeismicMagnitude[] = "seismic_magnitude";
constexpr char Ultraviolet[] = "ultraviolet";
constexpr char ElectricalResistivity[] = "electrical_resistivity";
constexpr char ElectricalConductivity[] = "electrical_conductivity";
constexpr char Loudness[] = "loudness";
constexpr char Moisture[] = "moisture";
constexpr char Frequency[] = "frequency";
constexpr char Time[] = "time";
constexpr char TargetTemperature[] = "target_temperature";
constexpr char ParticulateMatter_2_5[] = "particulate_matter_2_dot_5";
constexpr char FormaldehydeLevel[] = "formaldehyde_level";
constexpr char RadonConcentration[] = "radon_concentration";
constexpr char MethaneDensity[] = "methane_density";
constexpr char VolatileOrganicCompoundLevel[] = "volatile_organic_compound_level";
constexpr char CarbonMonoxideLevel[] = "co_level";
constexpr char SoilHumidity[] = "soil_humidity";
constexpr char SoilReactivity[] = "soil_reactivity";
constexpr char SoilSalinity[] = "soil_salinity";
constexpr char HeartRate[] = "heart_rate";
constexpr char BloodPressure[] = "blood_pressure";
constexpr char MuscleMass[] = "muscle_mass";
constexpr char FatMass[] = "fat_mass";
constexpr char BoneMass[] = "bone_mass";
constexpr char TotalBodyWater[] = "total_body_water";
constexpr char BasisMetabolicRate[] = "basal_metabolic_rate";
constexpr char BodyMassIndex[] = "body_mass_index";
constexpr char AccelerationXAxis[] = "acceleration_x_axis";
constexpr char AccelerationYAxis[] = "acceleration_y_axis";
constexpr char AccelerationZAxis[] = "acceleration_z_axis";
constexpr char SmokeDensity[] = "smoke_density";
constexpr char WaterFlow[] = "water_flow";
constexpr char WaterPressure[] = "water_pressure";
constexpr char RFSignalStrength[] = "rf_signal_strength";
constexpr char ParticulateMatter_10[] = "particulate_matter_10";
constexpr char RespiratoryRate[] = "respiratory_rate";
constexpr char RelativeModulationLevel[] = "relative_modulation_level";
constexpr char BoilerWaterTemperature[] = "boiler_water_temperature";
constexpr char DomesticHotWaterTemperature[] = "domestic_hot_water_temperature";
constexpr char OutsideTemperature[] = "outside_temperature";
constexpr char ExhaustTemperature[] = "exhaust_temperature";
constexpr char WaterChlorineLevel[] = "water_chlorine_level";
constexpr char WaterAcidity[] = "water_acidity";
constexpr char WaterOxidationReductionPotential[] = "water_oxidation_reduction_potential";
constexpr char HeartRate_LFHF_Ratio[] = "heart_rate_lf_hf_ratio";
constexpr char MotionDirection[] = "motion_direction";
constexpr char AppliedForceOnSensor[] = "applied_force_on_sensor";
constexpr char ReturnAirTemperature[] = "return_air_temperature";
constexpr char SupplyAirTemperature[] = "supply_air_temperature";
constexpr char CondenserCoilTemperature[] = "condenser_coil_temperature";
constexpr char EvaporatorCoilTemperature[] = "evaporator_coil_temperature";
constexpr char LiquidLineTemperature[] = "liquid_line_temperature";
constexpr char DischargeLineTemperature[] = "discharge_line_temperature";
constexpr char SuctionPressure[] = "suction_pressure";
constexpr char DischargePressure[] = "discharge_pressure";
constexpr char DefrostTemperature[] = "defrost_temperature";
constexpr char Ozone[] = "ozone";
constexpr char SulfurDioxide[] = "sulfur_dioxide";
constexpr char NitrogenDioxide[] = "nitrogen_dioxide";
constexpr char Ammonia[] = "ammonia";
constexpr char Lead[] = "lead";
constexpr char ParticulateMatter_1[] = "particulate_matter_1";
}  // namespace sensor_types

}  // namespace cc_sensor_multilevel

namespace cc_sensor_binary {

constexpr char Name[] = "cc_0x30_sensor_binary";
}

namespace cc_meter {

constexpr char Name[] = "cc_0x32_meter";
}

namespace cc_switch_color {

constexpr char Name[] = "cc_0x33_color_switch";
constexpr char Lead[] = "lead";

constexpr char Wwhite[] = "wwhite";
constexpr char Cwhite[] = "cwhite";
constexpr char Red[] = "red";
constexpr char Green[] = "green";
constexpr char Blue[] = "blue";
constexpr char Amber[] = "amber";
constexpr char Cyan[] = "cyan";
constexpr char Purple[] = "purple";
constexpr char Indexed[] = "indexed";
}  // namespace cc_switch_color

namespace cc_thermostat_mode {

constexpr char Name[] = "cc_0x40_thermostat_mode";
}

namespace cc_thermostat_operating_state {

constexpr char Name[] = "cc_0x42_thermostat_operating_state";

namespace type {

constexpr char Idle[] = "idle";
constexpr char Heating[] = "heating";
constexpr char Cooling[] = "cooling";
constexpr char FanOnly[] = "fan_only";
constexpr char PendingHeat[] = "pending_heat";
constexpr char PendingCool[] = "pending_cool";
constexpr char VentEco[] = "vent_economizer";
constexpr char AuxHeating[] = "aux_heating";
constexpr char SecStageHeating[] = "2nd_stage_heating";
constexpr char SecStageCooling[] = "2nd_stage_cooling";
constexpr char SecStageAuxHeat[] = "2nd_stage_aux_heat";
constexpr char ThirdStageAuxHeat[] = "3rd_stage_aux_heat";

}  // namespace type
}  // namespace cc_thermostat_operating_state

namespace cc_barrier_operator_state {

constexpr char Name[] = "cc_0x66_barrier_operator_state";

namespace type {

constexpr char Closed[] = "barrier_closed";
constexpr char Closing[] = "operating_closing";
constexpr char Stopped[] = "operating_unknown";
constexpr char Opening[] = "operating_opening";
constexpr char Opened[] = "barrier_opened";

}  // namespace type
}  // namespace cc_barrier_operator_state

namespace cc_barrier_operator_signal {

constexpr char AudibleNotification[] = "audible_notification";
constexpr char VisualNotification[] = "visual_notification";

}  // namespace cc_barrier_operator_signal

namespace cc_configuration_format {

constexpr char SignedInteger[] = "signed_integer";
constexpr char UnsignedInteger[] = "unsigned_integer";
constexpr char Enumerated[] = "enumerated";
constexpr char BitField[] = "bit_field";

}  // namespace cc_configuration_format

namespace cc_thermostat_setpoint {

constexpr char Name[] = "cc_0x43_thermostat_setpoint";
}

namespace cc_thermostat_fan_mode {

constexpr char Name[] = "cc_0x44_thermostat_fan_mode";
}

namespace cc_thermostat_fan_state {

constexpr char Name[] = "cc_0x45_thermostat_fan_state";
}

namespace cc_door_lock {

constexpr char Name[] = "cc_0x62_door_lock";

namespace modes {

constexpr char Unsecured[] = "unsecured";
constexpr char Unsecured_WithTimeout[] = "unsecured_with_timeout";
constexpr char Unsecured_InsideHandles[] = "unsecured_for_inside";
constexpr char Unsecured_InsideHandles_WithTimeout[] = "unsecured_for_inside_with_timeout";
constexpr char Unsecured_OutsideHandles[] = "unsecured_for_outside";
constexpr char Unsecured_OutsideHandles_WithTimeout[] = "unsecured_for_outside_with_timeout";
constexpr char Unknown[] = "unknown";
constexpr char Secured[] = "secured";
}  // namespace modes

namespace door_condition {
namespace latch_states {
constexpr char LatchOpen[] = "latch_open";
constexpr char LatchClosed[] = "latch_closed";
}  // namespace latch_states

namespace bolt_states {
constexpr char BoltLocked[] = "bolt_locked";
constexpr char BoltUnlocked[] = "bolt_unlocked";
}  // namespace bolt_states

namespace door_states {
constexpr char DoorOpen[] = "door_open";
constexpr char DoorClosed[] = "door_closed";
}  // namespace door_states
}  // namespace door_condition
}  // namespace cc_door_lock

namespace cc_switch_multilevel {

constexpr char Name[] = "cc_0x26_switch_multi_level";

}  // namespace cc_switch_multilevel

namespace value_change_command {

constexpr char Up[] = "up";
constexpr char Down[] = "down";
constexpr char Stop[] = "stop";

}  // namespace value_change_command

namespace cc_user_code {

constexpr char Name[] = "cc_0x63_user_code";

namespace UserIdStatus {

constexpr char Available[] = "available";
constexpr char Enabled[] = "enabled";
constexpr char Disabled[] = "disabled";
constexpr char Messaging[] = "messaging";
constexpr char PassageMode[] = "passage";
constexpr char NotAvailable[] = "not_available";

}  // namespace UserIdStatus

namespace KeypadMode {
constexpr char Normal[] = "normal";
constexpr char Vacation[] = "vacation";
constexpr char Privacy[] = "privacy";
constexpr char LockedOut[] = "locked_out";
}  // namespace KeypadMode

namespace UserPollingStatus {

constexpr char Idle[] = "idle";
constexpr char Polling[] = "polling";
constexpr char Finished[] = "finished";

}  // namespace UserPollingStatus
}  // namespace cc_user_code

namespace zwave_plus_icons {

constexpr char IconTypeGenericLightDimmerSwitch[] = "ICON_TYPE_GENERIC_LIGHT_DIMMER_SWITCH";
constexpr char IconTypeSpecificLightDimmerSwitchWallLamp[] = "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_WALL_LAMP";
constexpr char IconTypeSpecificLightDimmerSwitchLampPostHigh[] =
    "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_LAMP_POST_HIGH";
constexpr char IconTypeSpecificLightDimmerSwitchLampPostLow[] = "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_LAMP_POST_LOW";
constexpr char IconTypeGenericOnOffPowerSwitch[] = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH";
constexpr char IconTypeSpecificOnOffPowerSwitchWallLamp[] = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_LAMP";
constexpr char IconTypeSpecificOnOffPowerSwitchLampPostHigh[] = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_LAMP_POST_HIGH";
constexpr char IconTypeSpecificOnOffPowerSwitchLampPostLow[] = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_LAMP_POST_LOW";
constexpr char IconTypeSpecificLightDimmerSwitchPlugin[] = "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_PLUGIN";
constexpr char IconTypeSpecificLightDimmerSwitchWallOutlet[] = "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_WALL_OUTLET";
constexpr char IconTypeSpecificLightDimmerSwitchCeilingOutlet[] =
    "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_CEILING_OUTLET";
constexpr char IconTypeGenericFanSwitch[] = "ICON_TYPE_GENERIC_FAN_SWITCH";
constexpr char IconTypeGenericDimmerWallSwitch[] = "ICON_TYPE_GENERIC_DIMMER_WALL_SWITCH";
constexpr char IconTypeSpecificDimmerWallSwitchOneButton[] = "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_ONE_BUTTON";
constexpr char IconTypeSpecificDimmerWallSwitchTwoButtons[] = "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_TWO_BUTTONS";
constexpr char IconTypeSpecificDimmerWallSwitchThreeButtons[] = "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_THREE_BUTTONS";
constexpr char IconTypeSpecificDimmerWallSwitchFourButtons[] = "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_FOUR_BUTTONS";
constexpr char IconTypeSpecificDimmerWallSwitchOneRotary[] = "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_ONE_ROTARY";
constexpr char IconTypeSpecificOnOffPowerSwitchPlugin[] = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_PLUGIN";
constexpr char IconTypeSpecificOnOffPowerSwitchWallOutlet[] = "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_OUTLET";
constexpr char IconTypeSpecificOnOffPowerSwitchCeilingOutlet[] =
    "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_CEILING_OUTLET";
constexpr char IconTypeGenericOnOffWallSwitch[] = "ICON_TYPE_GENERIC_ON_OFF_WALL_SWITCH";
constexpr char IconTypeSpecificOnOffWallSwitchOneButton[] = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_BUTTON";
constexpr char IconTypeSpecificOnOffWallSwitchTwoButtons[] = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_TWO_BUTTONS";
constexpr char IconTypeSpecificOnOffWallSwitchThreeButtons[] = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_THREE_BUTTONS";
constexpr char IconTypeSpecificOnOffWallSwitchFourButtons[] = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_FOUR_BUTTONS";
constexpr char IconTypeSpecificOnOffWallSwitchOneRotary[] = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_ROTARY";
constexpr char IconTypeGenericValveOpenClose[] = "ICON_TYPE_GENERIC_VALVE_OPEN_CLOSE";
constexpr char IconTypeGenericIrrigation[] = "ICON_TYPE_GENERIC_IRRIGATION";
constexpr char IconTypeSpecificLightDimmerSwitchDinRailModule[] =
    "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_DIN_RAIL_MODULE";
constexpr char IconTypeSpecificOnOffPowerSwitchDinRailModule[] =
    "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_DIN_RAIL_MODULE";
constexpr char IconTypeGenericThermostat[] = "ICON_TYPE_GENERIC_THERMOSTAT";
constexpr char IconTypeGenericDoorLockKeypad[] = "ICON_TYPE_GENERIC_DOOR_LOCK_KEYPAD";
constexpr char IconTypeGenericEntryControl[] = "ICON_TYPE_GENERIC_ENTRY_CONTROL";
constexpr char IconTypeSpecificEntryControlRfidTagReaderNoButton[] =
    "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_RFID_TAG_READER_NO_BUTTON";
constexpr char IconTypeGenericWindowCoveringNoPositionEndpoint[] =
    "ICON_TYPE_GENERIC_WINDOW_COVERING_NO_POSITION_ENDPOINT";
constexpr char IconTypeGenericWindowCoveringEndpointAware[] = "ICON_TYPE_GENERIC_WINDOW_COVERING_ENDPOINT_AWARE";
constexpr char IconTypeGenericWindowCoveringPositionEndpointAware[] =
    "ICON_TYPE_GENERIC_WINDOW_COVERING_POSITION_ENDPOINT_AWARE";
constexpr char IconTypeGenericRemoteControlAv[] = "ICON_TYPE_GENERIC_REMOTE_CONTROL_AV";
constexpr char IconTypeGenericRemoteControlMultiPurpose[] = "ICON_TYPE_GENERIC_REMOTE_CONTROL_MULTI_PURPOSE";
constexpr char IconTypeGenericRemoteControlSimple[] = "ICON_TYPE_GENERIC_REMOTE_CONTROL_SIMPLE";
constexpr char IconTypeSpecificRemoteControlSimpleKeyfob[] = "ICON_TYPE_SPECIFIC_REMOTE_CONTROL_SIMPLE_KEYFOB";
constexpr char IconTypeGenericWallController[] = "ICON_TYPE_GENERIC_WALL_CONTROLLER";
constexpr char IconTypeGenericCentralController[] = "ICON_TYPE_GENERIC_CENTRAL_CONTROLLER";
constexpr char IconTypeGenericDisplaySimple[] = "ICON_TYPE_GENERIC_DISPLAY_SIMPLE";
constexpr char IconTypeGenericGateway[] = "ICON_TYPE_GENERIC_GATEWAY";
constexpr char IconTypeGenericSetTopBox[] = "ICON_TYPE_GENERIC_SET_TOP_BOX";
constexpr char IconTypeGenericSubSystemController[] = "ICON_TYPE_GENERIC_SUB_SYSTEM_CONTROLLER";
constexpr char IconTypeGenericTv[] = "ICON_TYPE_GENERIC_TV";
constexpr char IconTypeGenericRepeater[] = "ICON_TYPE_GENERIC_REPEATER";
constexpr char IconTypeSpecificRepeaterSlave[] = "ICON_TYPE_SPECIFIC_REPEATER_SLAVE";
constexpr char IconTypeSpecificIrRepeater[] = "ICON_TYPE_SPECIFIC_IR_REPEATER";
constexpr char IconTypeGenericPowerStrip[] = "ICON_TYPE_GENERIC_POWER_STRIP";
constexpr char IconTypeGenericSiren[] = "ICON_TYPE_GENERIC_SIREN";
constexpr char IconTypeGenericSoundSwitch[] = "ICON_TYPE_GENERIC_SOUND_SWITCH";
constexpr char IconTypeSpecificSoundSwitchChime[] = "ICON_TYPE_SPECIFIC_SOUND_SWITCH_CHIME";
constexpr char IconTypeSpecificOnOffWallSwitchDoorBell[] = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_DOOR_BELL";
constexpr char IconTypeSpecificSoundSwitchDoorbell[] = "ICON_TYPE_SPECIFIC_SOUND_SWITCH_DOORBELL";
constexpr char IconTypeSpecificEntryControlKeypad09[] = "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_KEYPAD_0_9";
constexpr char IconTypeSpecificEntryControlKeypad09OkCancel[] = "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_KEYPAD_0_9_OK_CANCEL";
constexpr char IconTypeSpecificEntryControlKeypad09OkCancelHomeStayAway[] =
    "ICON_TYPE_SPECIFIC_ENTRY_CONTROL_KEYPAD_0_9_OK_CANCEL_HOME_STAY_AWAY";
constexpr char IconTypeGenericBarrier[] = "ICON_TYPE_GENERIC_BARRIER";
constexpr char IconTypeGenericSensorNotification[] = "ICON_TYPE_GENERIC_SENSOR_NOTIFICATION";
constexpr char IconTypeSpecificSensorNotificationSmokeAlarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_SMOKE_ALARM";
constexpr char IconTypeSpecificSensorNotificationCoAlarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_CO_ALARM";
constexpr char IconTypeSpecificSensorNotificationCo2Alarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_CO2_ALARM";
constexpr char IconTypeSpecificSensorNotificationHeatAlarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_HEAT_ALARM";
constexpr char IconTypeSpecificSensorNotificationWaterAlarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_WATER_ALARM";
constexpr char IconTypeSpecificSensorNotificationDoorWindow[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_DOOR_WINDOW";
constexpr char IconTypeSpecificSensorNotificationMotionAlarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MOTION_ALARM";
constexpr char IconTypeSpecificSensorNotificationPowerManagement[] =
    "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_POWER_MANAGEMENT";
constexpr char IconTypeSpecificSensorNotificationSystem[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_SYSTEM";
constexpr char IconTypeSpecificSensorNotificationEmergencyAlarm[] =
    "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_EMERGENCY_ALARM";
constexpr char IconTypeSpecificSensorNotificationClock[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_CLOCK";
constexpr char IconTypeSpecificSensorNotificationAppliance[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_APPLIANCE";
constexpr char IconTypeSpecificSensorNotificationHomeHealth[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_HOME_HEALTH";
constexpr char IconTypeSpecificSensorNotificationSiren[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_SIREN";
constexpr char IconTypeSpecificSensorNotificationWaterValve[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_WATER_VALVE";
constexpr char IconTypeSpecificSensorNotificationWeatherAlarm[] =
    "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_WEATHER_ALARM";
constexpr char IconTypeSpecificSensorNotificationIrrigation[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_IRRIGATION";
constexpr char IconTypeSpecificSensorNotificationGasAlarm[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_GAS_ALARM";
constexpr char IconTypeSpecificSensorNotificationPestControl[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_PEST_CONTROL";
constexpr char IconTypeSpecificSensorNotificationLightSensor[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_LIGHT_SENSOR";
constexpr char IconTypeSpecificSensorNotificationMultidevice[] = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MULTIDEVICE";

}  // namespace zwave_plus_icons

namespace generic_types {

constexpr char Unknown[] = "GENERIC_TYPE_UNKNOWN";
constexpr char GenericTypeSwitchMultilevel[] = "GENERIC_TYPE_SWITCH_MULTILEVEL";
constexpr char GenericTypeSwitchBinary[] = "GENERIC_TYPE_SWITCH_BINARY";
constexpr char GenericTypeSwitchRemote[] = "GENERIC_TYPE_SWITCH_REMOTE";
constexpr char GenericTypeSwitchToggle[] = "GENERIC_TYPE_SWITCH_TOGGLE";
constexpr char GenericTypeThermostat[] = "GENERIC_TYPE_THERMOSTAT";
constexpr char GenericTypeEntryControl[] = "GENERIC_TYPE_ENTRY_CONTROL";
constexpr char GenericTypeWindowCovering[] = "GENERIC_TYPE_WINDOW_COVERING";
constexpr char GenericTypeGenericController[] = "GENERIC_TYPE_GENERIC_CONTROLLER";
constexpr char GenericTypeWallController[] = "GENERIC_TYPE_WALL_CONTROLLER";
constexpr char GenericTypeStaticController[] = "GENERIC_TYPE_STATIC_CONTROLLER";
constexpr char GenericTypeRepeaterSlave[] = "GENERIC_TYPE_REPEATER_SLAVE";
constexpr char GenericTypeNetworkExtender[] = "GENERIC_TYPE_NETWORK_EXTENDER";
constexpr char GenericTypeAvControlPoint[] = "GENERIC_TYPE_AV_CONTROL_POINT";
constexpr char GenericTypeAppliance[] = "GENERIC_TYPE_APPLIANCE";
constexpr char GenericTypeSensorNotification[] = "GENERIC_TYPE_SENSOR_NOTIFICATION";
constexpr char GenericTypeSecurityPanel[] = "GENERIC_TYPE_SECURITY_PANEL";
constexpr char GenericTypeSensorBinary[] = "GENERIC_TYPE_SENSOR_BINARY";
constexpr char GenericTypeSensorMultilevel[] = "GENERIC_TYPE_SENSOR_MULTILEVEL";
constexpr char GenericTypeSensorAlarm[] = "GENERIC_TYPE_SENSOR_ALARM";

}  // namespace generic_types

namespace specific_types {

constexpr char Unknown[] = "SPECIFIC_TYPE_UNKNOWN";
constexpr char SpecificTypePowerSwitchMultilevel[] = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL";
constexpr char SpecificTypePowerSwitchBinary[] = "SPECIFIC_TYPE_POWER_SWITCH_BINARY";
constexpr char SpecificTypeNotUsed[] = "SPECIFIC_TYPE_NOT_USED";
constexpr char SpecificTypeSceneSwitchMultilevel[] = "SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL";
constexpr char SpecificTypeFanSwitch[] = "SPECIFIC_TYPE_FAN_SWITCH";
constexpr char SpecificTypeSwitchRemoteMultilevel[] = "SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL";
constexpr char SpecificTypeSwitchRemoteToggleMultilevel[] = "SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL";
constexpr char SpecificTypeSwitchToggleMultilevel[] = "SPECIFIC_TYPE_SWITCH_TOGGLE_MULTILEVEL";
constexpr char SpecificTypeColorTunableMultilevel[] = "SPECIFIC_TYPE_COLOR_TUNABLE_MULTILEVEL";
constexpr char SpecificTypeColorTunableBinary[] = "SPECIFIC_TYPE_COLOR_TUNABLE_BINARY";
constexpr char SpecificTypeSceneSwitchBinary[] = "SPECIFIC_TYPE_SCENE_SWITCH_BINARY";
constexpr char SpecificTypeSwitchRemoteBinary[] = "SPECIFIC_TYPE_SWITCH_REMOTE_BINARY";
constexpr char SpecificTypeSwitchRemoteToggleBinary[] = "SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY";
constexpr char SpecificTypeSwitchToggleBinary[] = "SPECIFIC_TYPE_SWITCH_TOGGLE_BINARY";
constexpr char SpecificTypeValveOpenClose[] = "SPECIFIC_TYPE_VALVE_OPEN_CLOSE";
constexpr char SpecificTypeIrrigationController[] = "SPECIFIC_TYPE_IRRIGATION_CONTROLLER";
constexpr char SpecificTypeThermostatGeneral[] = "SPECIFIC_TYPE_THERMOSTAT_GENERAL";
constexpr char SpecificTypeThermostatGeneralV2[] = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2";
constexpr char SpecificTypeThermostatHeating[] = "SPECIFIC_TYPE_THERMOSTAT_HEATING";
constexpr char SpecificTypeDoorLock[] = "SPECIFIC_TYPE_DOOR_LOCK";
constexpr char SpecificTypeAdvancedDoorLock[] = "SPECIFIC_TYPE_ADVANCED_DOOR_LOCK";
constexpr char SpecificTypeSecureKeypadDoorLock[] = "SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK";
constexpr char SpecificTypeSecureKeypadDoorLockDeadbolt[] = "SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK_DEADBOLT";
constexpr char SpecificTypeSecureLockbox[] = "SPECIFIC_TYPE_SECURE_LOCKBOX";
constexpr char SpecificTypeClassAMotorControl[] = "SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL";
constexpr char SpecificTypeClassBMotorControl[] = "SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL";
constexpr char SpecificTypeClassCMotorControl[] = "SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL";
constexpr char SpecificTypeMotorMultiposition[] = "SPECIFIC_TYPE_MOTOR_MULTIPOSITION";
constexpr char SpecificTypeSimpleWindowCovering[] = "SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING";
constexpr char SpecificTypePortableRemoteController[] = "SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER";
constexpr char SpecificTypePortableSceneController[] = "SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER";
constexpr char SpecificTypePortableInstallerTool[] = "SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL";
constexpr char SpecificTypeRemoteControlAv[] = "SPECIFIC_TYPE_REMOTE_CONTROL_AV";
constexpr char SpecificTypeRemoteControlSimple[] = "SPECIFIC_TYPE_REMOTE_CONTROL_SIMPLE";
constexpr char SpecificTypeBasicWallController[] = "SPECIFIC_TYPE_BASIC_WALL_CONTROLLER";
constexpr char SpecificTypePcController[] = "SPECIFIC_TYPE_PC_CONTROLLER";
constexpr char SpecificTypeSceneController[] = "SPECIFIC_TYPE_SCENE_CONTROLLER";
constexpr char SpecificTypeStaticInstallerTool[] = "SPECIFIC_TYPE_STATIC_INSTALLER_TOOL";
constexpr char SpecificTypeSetTopBox[] = "SPECIFIC_TYPE_SET_TOP_BOX";
constexpr char SpecificTypeSubSystemController[] = "SPECIFIC_TYPE_SUB_SYSTEM_CONTROLLER";
constexpr char SpecificTypeTv[] = "SPECIFIC_TYPE_TV";
constexpr char SpecificTypeGateway[] = "SPECIFIC_TYPE_GATEWAY";
constexpr char SpecificTypeRepeaterSlave[] = "SPECIFIC_TYPE_REPEATER_SLAVE";
constexpr char SpecificTypeVirtualNode[] = "SPECIFIC_TYPE_VIRTUAL_NODE";
constexpr char SpecificTypeSecureExtender[] = "SPECIFIC_TYPE_SECURE_EXTENDER";
constexpr char SpecificTypePowerStrip[] = "SPECIFIC_TYPE_POWER_STRIP";
constexpr char SpecificTypeSatelliteReceiver[] = "SPECIFIC_TYPE_SATELLITE_RECEIVER";
constexpr char SpecificTypeSatelliteReceiverV2[] = "SPECIFIC_TYPE_SATELLITE_RECEIVER_V2";
constexpr char SpecificTypeSiren[] = "SPECIFIC_TYPE_SIREN";
constexpr char SpecificTypeGeneralAppliance[] = "SPECIFIC_TYPE_GENERAL_APPLIANCE";
constexpr char SpecificTypeKitchenAppliance[] = "SPECIFIC_TYPE_KITCHEN_APPLIANCE";
constexpr char SpecificTypeLaundryAppliance[] = "SPECIFIC_TYPE_LAUNDRY_APPLIANCE";
constexpr char SpecificTypeNotificationSensor[] = "SPECIFIC_TYPE_NOTIFICATION_SENSOR";
constexpr char SpecificTypeDoorbell[] = "SPECIFIC_TYPE_DOORBELL";
constexpr char SpecificTypeSoundSwitch[] = "SPECIFIC_TYPE_SOUND_SWITCH";
constexpr char SpecificTypeSecureKeypad[] = "SPECIFIC_TYPE_SECURE_KEYPAD";
constexpr char SpecificTypeZonedSecurityPanel[] = "SPECIFIC_TYPE_ZONED_SECURITY_PANEL";
constexpr char SpecificTypeSecureDoor[] = "SPECIFIC_TYPE_SECURE_DOOR";
constexpr char SpecificTypeSecureGate[] = "SPECIFIC_TYPE_SECURE_GATE";
constexpr char SpecificTypeSecureBarrierAddon[] = "SPECIFIC_TYPE_SECURE_BARRIER_ADDON";
constexpr char SpecificTypeSecureBarrierOpenOnly[] = "SPECIFIC_TYPE_SECURE_BARRIER_OPEN_ONLY";
constexpr char SpecificTypeSecureBarrierCloseOnly[] = "SPECIFIC_TYPE_SECURE_BARRIER_CLOSE_ONLY";
constexpr char SpecificTypeRoutingSensorBinary[] = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY";
constexpr char SpecificTypeRoutingSensorMultilevel[] = "SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL";

}  // namespace specific_types

namespace cc_central_scene_attributes {

constexpr char Unknown[] = "unknown";
constexpr char Press1Time[] = "press_1_time";
constexpr char Released[] = "released";
constexpr char HeldDown[] = "held_down";
constexpr char Press2Times[] = "press_2_times";
constexpr char Press3Times[] = "press_3_times";
constexpr char Press4Times[] = "press_4_times";
constexpr char Press5Times[] = "press_5_times";

}  // namespace cc_central_scene_attributes

namespace security_modes {

constexpr char none[] = "none";
constexpr char S2[] = "accessControl";
constexpr char S2Authenticated[] = "authenticated";
constexpr char S2Unauthenticated[] = "unauthenticated";
constexpr char S0[] = "s0";
constexpr char CSA[] = "clientSide";

}  // namespace security_modes

namespace provisionong {

constexpr char S2[] = "S2";
constexpr char Smart[] = "Smart";

}  // namespace provisionong

namespace agi_profiles {

constexpr char General[] = "General";
constexpr char Control[] = "Control";
constexpr char Sensor[] = "Sensor";
constexpr char Notification[] = "Notification";
constexpr char Meter[] = "Meter";
constexpr char Irrigation[] = "Irrigation";

constexpr char NA[] = "NA";
constexpr char Lifeline[] = "Lifeline";
constexpr char Key[] = "Key";
constexpr char Channel[] = "Channel";

}  // namespace agi_profiles

}  // namespace resources
}  // namespace znet
