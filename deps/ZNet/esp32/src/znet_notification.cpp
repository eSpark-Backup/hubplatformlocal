#include <initializer_list>
#include <cassert>
#include <algorithm>
#include <sstream>

#include <znet/znet_notification.hpp>
#include <znet/znet_utils.hpp>

namespace znet {

using namespace utils;

namespace {

using RangedStringList = RangedList<std::uint8_t, const char*>;

class ValueMappingHandler : public NotificationEventData::ValueHandler {
public:
    ValueMappingHandler( RangedStringList list )
    : m_list( list ) {}

    NotificationValueOpt getNotificationValue( const std::uint8_t* value, std::uint8_t size ) const override {
        if( !value || size == 0 ) {
            return {};
        }

        const auto result = findInRangedList( m_list, *value );

        return result ? NotificationValueOpt( std::string( *result ) ) : NotificationValueOpt();
    }

private:
    RangedStringList m_list;
};

class UintValueHandler : public NotificationEventData::ValueHandler {
public:
    NotificationValueOpt getNotificationValue( const std::uint8_t* data, std::uint8_t size ) const override {
        if( !data || size == 0 ) {
            return {};
        }

        return NotificationValueOpt( data[0] );
    }
};

class LocationValueHandler : public NotificationEventData::ValueHandler {
public:
    NotificationValueOpt getNotificationValue( const std::uint8_t*, std::uint8_t ) const override {
        // TODO: implement
        return {};
    }
};

class ManufacturerValueHandler : public NotificationEventData::ValueHandler {
public:
    NotificationValueOpt getNotificationValue( const std::uint8_t*, std::uint8_t ) const override {
        // TODO: implement
        return {};
    }
};

class TimeValueHandler : public NotificationEventData::ValueHandler {
public:
    NotificationValueOpt getNotificationValue( const std::uint8_t* data, std::uint8_t size ) const override {
        if( !data || size < 3u ) {
            return {};
        }

        return NotificationValueOpt( NotificationTimeValue{ data[0], data[1], data[2] } );
    }
};

class UserCodeReportValueHandler : public NotificationEventData::ValueHandler {
public:
    /* User Code Report (User Code Command Class V1)
     * [0] = COMMAND_CLASS_USER_CODE (0x63)
     * [1] = USER_CODE_REPORT (0x03)
     * [2] = User Identifier
     * [3] = User ID Status
     * */
    NotificationValueOpt getNotificationValue( const std::uint8_t* data, std::uint8_t size ) const override {
        if( !data || size < 4u ) {
            return {};
        }

        return NotificationValueOpt( data[2] );
    }
};

template <NotificationType notificationType>
class IdleValueHandler : public NotificationEventData::ValueHandler {
public:
    NotificationValueOpt getNotificationValue( const std::uint8_t* data, std::uint8_t size ) const override {
        if( !data || size == 0 ) {
            return {};
        }

        const auto eventData = getNotificationEventData( notificationType, *data );
        return eventData ? NotificationValueOpt( std::string( eventData->m_name ) ) : NotificationValueOpt();
    }
};

using EventDataList = std::initializer_list<const NotificationEventData>;
using EventsRangeList = RangedList<NotificationEvent, NotificationEventData>;
using StringList = std::initializer_list<const char* const>;
using NotificationEventsList = RangedList<NotificationType, EventsRangeList>;

static const NotificationEventsList& getNotificationEventsList() {
    static const StringList TestResultList_01 = {
        "test_ok",     // 0x01
        "test_failed"  // 0x02
    };
    static const RangedStringList TestResultList = { { 0x01, TestResultList_01 } };
    static const ValueMappingHandler TestResultHandler = { TestResultList };

    static const StringList ThresholdResultList_01 = {
        "no_data",               // 0x01
        "below_low_threshold",   // 0x02
        "above_high_threshold",  // 0x03
        "max"                    // 0x04
    };
    static const RangedStringList ThresholdResultList = { { 0x01, ThresholdResultList_01 } };
    static const ValueMappingHandler ThresholdResultHandler = { ThresholdResultList };

    static const StringList PollutionLevelList_01 = {
        "clean",                // 0x01
        "slightly_polluted",    // 0x02
        "moderately_polluted",  // 0x03
        "highly_polluted",      // 0x04
    };
    static const RangedStringList PollutionLevelList = { { 0x01, PollutionLevelList_01 } };
    static const ValueMappingHandler PollutionLevelHandler = { PollutionLevelList };

    static const StringList BreathLevelList_01 = {
        "low_breath",  // 0x01
        "no_breath",   // 0x02
    };
    static const RangedStringList BreathLevelList = { { 0x01, BreathLevelList_01 } };
    static const ValueMappingHandler BreathLevelHandler = { BreathLevelList };

    static const StringList OnOffList_01 = {
        "off",  // 0x01
        "on",   // 0x02
    };
    static const RangedStringList OnOffList = { { 0x01, OnOffList_01 } };
    static const ValueMappingHandler OnOffHandler = { OnOffList };

    static const StringList Completed = { "completed" };
    static const StringList InProgress = { "in_progress" };
    static const RangedStringList ProcessStatusList = { { 0x00, Completed }, { 0xFF, InProgress } };
    static const ValueMappingHandler ProcessStatusHandler = { ProcessStatusList };

    static const StringList Disabled = { "disabled" };
    static const StringList Enabled = { "enabled" };
    static const RangedStringList EnableStateList = { { 0x00, Disabled }, { 0xFF, Enabled } };
    static const ValueMappingHandler EnableStateHandler = { EnableStateList };

    static const StringList Yes = { "yes" };
    static const StringList No = { "no" };
    static const RangedStringList YesNoList = { { 0x00, No }, { 0xFF, Yes } };
    static const ValueMappingHandler YesNoHandler = { YesNoList };

    static const StringList WaterQualityThresholdList_01 = {
        "below_low_threshold",   // 0x01
        "above_high_threshold",  // 0x02
    };
    static const RangedStringList WaterQualityThresholdList = { { 0x01, WaterQualityThresholdList_01 } };
    static const ValueMappingHandler WaterQualityThresholdHandler = { WaterQualityThresholdList };

    static const StringList PhList_03 = {
        "decreasing_ph",  // 0x03
        "increasing_ph",  // 0x04
    };
    static const RangedStringList PhList = { { 0x01, WaterQualityThresholdList_01 }, { 0x03, PhList_03 } };
    static const ValueMappingHandler PhHandler = { PhList };

    static const LocationValueHandler LocationHandler;
    static const UintValueHandler UintHandler;
    static const ManufacturerValueHandler ManufacturerHandler;
    static const TimeValueHandler TimeHandler;
    static const UserCodeReportValueHandler UserCodeReportHandler;

    static const IdleValueHandler<NotificationType::SmokeAlarm> SmokeAlarmIdleHandler;

    static const EventDataList SmokeAlarmList_00 = {
        { "state_idle", 4, &SmokeAlarmIdleHandler },                         // 0x00
        { "smoke_detected", 2, &LocationHandler },                           // 0x01
        { "smoke_detected", 2, nullptr },                                    // 0x02
        { "smoke_alarm_test", 3, nullptr },                                  // 0x03
        { "replacement_required", 5, nullptr },                              // 0x04
        { "device_end_of_life", 8, nullptr },                                // 0x05
        { "alarm_silenced", 8, nullptr },                                    // 0x06
        { "maintenance_required_planned_periodic_inspection", 8, nullptr },  // 0x07
        { "dust_detected", 8, nullptr },                                     // 0x08
    };
    static const EventsRangeList SmokeAlarm = { { 0x00, SmokeAlarmList_00 } };

    static const IdleValueHandler<NotificationType::COAlarm> CoAlarmIdleHandler;
    static const EventDataList CoAlarm_00 = {
        { "state_idle", 4, &CoAlarmIdleHandler },                            // 0x00
        { "co_detected", 2, &LocationHandler },                              // 0x01
        { "co_detected", 2, nullptr },                                       // 0x02
        { "carbon_monoxide_test", 5, &TestResultHandler },                   // 0x03
        { "replacement_required", 5, nullptr },                              // 0x04
        { "device_end_of_life", 8, nullptr },                                // 0x05
        { "alarm_silenced", 8, nullptr },                                    // 0x06
        { "maintenance_required_planned_periodic_inspection", 8, nullptr },  // 0x07
    };
    static const EventsRangeList CoAlarm = { { 0x00, CoAlarm_00 } };

    static const IdleValueHandler<NotificationType::CO2Alarm> Co2AlarmIdleHandler;
    static const EventDataList C02AlarmList_00 = {
        { "state_idle", 4, &Co2AlarmIdleHandler },                           // 0x00
        { "co2_detected", 2, &LocationHandler },                             // 0x01
        { "co2_detected", 2, nullptr },                                      // 0x02
        { "carbon_dioxide_test", 5, &TestResultHandler },                    // 0x03
        { "replacement_required", 5, nullptr },                              // 0x04
        { "device_end_of_life", 8, nullptr },                                // 0x05
        { "alarm_silenced", 8, nullptr },                                    // 0x06
        { "maintenance_required_planned_periodic_inspection", 8, nullptr },  // 0x07
    };
    static const EventsRangeList Co2Alarm = { { 0x00, C02AlarmList_00 } };

    static const IdleValueHandler<NotificationType::HeatAlarm> HeatAlarmIdleHandler;
    static const EventDataList HeatAlarmList_00 = {
        { "state_idle", 4, &HeatAlarmIdleHandler },                          // 0x00
        { "overheat_detected", 2, &LocationHandler },                        // 0x01
        { "overheat_detected", 2, nullptr },                                 // 0x02
        { "rapid_temperature_rise", 2, &LocationHandler },                   // 0x03
        { "rapid_temperature_rise", 2, nullptr },                            // 0x04
        { "under_heat_detected", 5, &LocationHandler },                      // 0x05
        { "under_heat_detected", 5, nullptr },                               // 0x06
        { "heat_alarm_test", 8, nullptr },                                   // 0x07
        { "device_end_of_life", 8, nullptr },                                // 0x08
        { "alarm_silenced", 8, nullptr },                                    // 0x09
        { "dust_detected", 8, nullptr },                                     // 0x0A
        { "maintenance_required_planned_periodic_inspection", 8, nullptr },  // 0x0B
        { "rapid_temperature_fail", 8, &LocationHandler },                   // 0x0C
        { "rapid_temperature_fail", 8, nullptr },                            // 0x0D
    };
    static const EventsRangeList HeatAlarm = { { 0x00, HeatAlarmList_00 } };

    static const IdleValueHandler<NotificationType::WaterAlarm> WaterAlarmIdleHandler;
    static const EventDataList WaterAlarm_00 = {
        { "state_idle", 4, &WaterAlarmIdleHandler },                // 0x00
        { "water_leak_detected", 2, &LocationHandler },             // 0x01
        { "water_leak_detected", 2, nullptr },                      // 0x02
        { "water_level_dropped", 2, &LocationHandler },             // 0x03
        { "water_level_dropped", 2, nullptr },                      // 0x04
        { "replace_water_filter", 4, nullptr },                     // 0x05
        { "water_flow_alarm", 7, &ThresholdResultHandler },         // 0x06
        { "water_pressure_alarm", 7, &ThresholdResultHandler },     // 0x07
        { "water_temperature_alarm", 8, &ThresholdResultHandler },  // 0x08
        { "water_level_alarm", 8, &ThresholdResultHandler },        // 0x09
        { "sump_pump_active", 8, nullptr },                         // 0x0A
        { "sump_pump_failure", 8, nullptr },                        // 0x0B
    };
    static const EventsRangeList WaterAlarm = { { 0x00, WaterAlarm_00 } };

    static const IdleValueHandler<NotificationType::AccessControl> AccessControlIdleHandler;
    static const EventDataList AccessControlList_00 = {
        { "state_idle", 4, &AccessControlIdleHandler },                    // 0x00
        { "manual_lock_operation", 2, nullptr },                           // 0x01
        { "manual_unlock_operation", 2, nullptr },                         // 0x02
        { "rf_lock_operation", 2, nullptr },                               // 0x03
        { "rf_unlock_operation", 2, nullptr },                             // 0x04
        { "keypad_lock_operation", 2, &UserCodeReportHandler },            // 0x05
        { "keypad_unlock_operation", 2, &UserCodeReportHandler },          // 0x06
        { "manual_not_fully_locked_operation", 3, nullptr },               // 0x07
        { "rf_not_fully_locked_operation", 3, nullptr },                   // 0x08
        { "auto_lock_locked_operation", 3, nullptr },                      // 0x09
        { "auto_lock_not_fully_locked_operation", 3, nullptr },            // 0x0A
        { "lock_jammed", 3, nullptr },                                     // 0x0B
        { "all_user_codes_deleted", 3, nullptr },                          // 0x0C
        { "single_user_code_deleted", 3, nullptr },                        // 0x0D
        { "new_user_code_added", 3, nullptr },                             // 0x0E
        { "new_user_code_not_added_due_to_duplicated_code", 3, nullptr },  // 0x0F
        { "keypad_temporary_disabled", 3, nullptr },                       // 0x10
        { "keypad_busy", 3, nullptr },                                     // 0x11
        { "new_program_code_entered", 3, nullptr },                        // 0x12
        { "manually_enter_user_code_exceeds_code_limit", 3, nullptr },     // 0x13
        { "unlock_by_rf_with_invalid_user_code", 3, nullptr },             // 0x14
        { "lock_by_rf_with_invalid_user_code", 3, nullptr },               // 0x15
        { "dw_is_opened", 3, nullptr },                                    // 0x16
        { "dw_is_closed", 3, nullptr },                                    // 0x17
        { "dw_handle_is_opened", 8, nullptr },                             // 0x18
        { "dw_handle_is_closed", 8, nullptr },                             // 0x19
    };

    static const EventDataList AccessControlList_20 = {
        { "messaging_user_code_entered_via_keypad", 8, nullptr /*TODO*/ },  // 0x20
    };

    static const EventDataList AccessControlList_40 = {
        { "barrier_performing_initialization_process", 4, &ProcessStatusHandler },               // 0x40
        { "barrier_operation_force_has_been_exceeded", 4, nullptr },                             // 0x41
        { "barrier_motor_exceeded_operational_time_limit", 4, nullptr /*TODO*/ },                // 0x42
        { "barrier_operation_exceeded_physical_mechanical_limits", 4, nullptr },                 // 0x43
        { "barrier_unable_to_perform_requested_operation_due_to_ul_requirements", 4, nullptr },  // 0x44
        { "barrier_unattended_operation_has_been_disabled_per_ul_requirements", 4, nullptr },    // 0x45
        { "barrier_failed_to_perform_requested_operation_device_malfunction", 4, nullptr },      // 0x46
        { "barrier_vacation_mode", 4, &EnableStateHandler },                                     // 0x47
        { "barrier_safety_beam_obstacle", 4, &YesNoHandler },                                    // 0x48
        { "barrier_sensor_not_detected", 4, &UintHandler },                                      // 0x49
        { "barrier_sensor_low_battery_warning", 4, &UintHandler },                               // 0x4A
        { "barrier_detected_short_in_wall_station_wires", 4, nullptr },                          // 0x4B
        { "barrier_associated_with_non_zwave_remote_control", 4, nullptr },                      // 0x4C
    };

    static const EventsRangeList AccessControl = {
        { 0x00, AccessControlList_00 }, { 0x20, AccessControlList_20 }, { 0x40, AccessControlList_40 }
    };

    static const IdleValueHandler<NotificationType::HomeSecurity> HomeSecurityIdleHandler;
    static const EventDataList HomeSecurityList_00 = {
        { "state_idle", 4, &HomeSecurityIdleHandler },     // 0x00
        { "intrusion", 2, &LocationHandler },              // 0x01
        { "intrusion", 2, nullptr },                       // 0x02
        { "product_cover_removed", 2, &LocationHandler },  // 0x03
        { "invalid_code", 2, nullptr },                    // 0x04
        { "glass_breakage", 2, &LocationHandler },         // 0x05
        { "glass_breakage", 2, nullptr },                  // 0x06
        { "motion_detected", 2, &LocationHandler },        // 0x07
        { "motion_detected", 4, nullptr },                 // 0x08
        { "product_moved", 6, nullptr },                   // 0x09
        { "impact_detected", 8, nullptr },                 // 0x0A
    };
    static const EventsRangeList HomeSecurity = { { 0x00, HomeSecurityList_00 } };

    static const IdleValueHandler<NotificationType::PowerManagement> PowerManagementIdleHandler;
    static const EventDataList PowerManagementList_00 = {
        { "state_idle", 4, &PowerManagementIdleHandler },  // 0x00
        { "power_has_been_applied", 2, nullptr },          // 0x01
        { "ac_mains_disconnected", 2, nullptr },           // 0x02
        { "ac_mains_reconnected", 2, &LocationHandler },   // 0x03
        { "surge_detected", 2, nullptr },                  // 0x04
        { "voltage_drop_drift", 2, nullptr },              // 0x05
        { "over_current_detected", 3, nullptr },           // 0x06
        { "over_voltage_detected", 3, nullptr },           // 0x07
        { "over_load_detected", 3, nullptr },              // 0x08
        { "load_error", 3, nullptr },                      // 0x09
        { "replace_battery_soon", 3, nullptr },            // 0x0A
        { "replace_battery_now", 3, nullptr },             // 0x0B
        { "battery_is_charging", 4, nullptr },             // 0x0C
        { "battery_is_fully_charged", 4, nullptr },        // 0x0D
        { "charge_battery_soon", 4, nullptr },             // 0x0E
        { "charge_battery_now", 4, nullptr },              // 0x0F
        { "backup_battery_is_low", 8, nullptr },           // 0x10
        { "battery_fluid_is_low", 8, nullptr },            // 0x11
        { "backup_battery_is_disconnected", 8, nullptr },  // 0x12
    };
    static const EventsRangeList PowerManagement = { { 0x00, PowerManagementList_00 } };

    static const IdleValueHandler<NotificationType::System> SystemIdleHandler;
    static const EventDataList SystemList_00 = {
        { "state_idle", 4, &SystemIdleHandler },                 // 0x00
        { "system_hardware_failure", 2, nullptr },               // 0x01
        { "system_software_failure", 2, nullptr },               // 0x02
        { "system_hardware_failure", 3, &ManufacturerHandler },  // 0x03
        { "system_software_failure", 3, &ManufacturerHandler },  // 0x04
        { "heartbeat", 5, nullptr },                             // 0x05
        { "product_cover_removed", 5, nullptr },                 // 0x06
        { "emergency_shutoff", 7, nullptr },                     // 0x07
    };

    static const EventDataList SystemList_09 = {
        { "digital_input_high_state", 7, nullptr },  // 0x09
        { "digital_input_low_state", 8, nullptr },   // 0x0A
        { "digital_input_open", 8, nullptr },        // 0x0B
    };

    static const EventsRangeList System = { { 0x00, SystemList_00 }, { 0x09, SystemList_09 } };

    static const IdleValueHandler<NotificationType::EmergencyAlarm> EmergencyAlarmIdleHandler;
    static const EventDataList EmergencyAlarmList_00 = {
        { "state_idle", 4, &EmergencyAlarmIdleHandler },  // 0x00
        { "contact_police", 2, nullptr },                 // 0x01
        { "contact_fire_service", 2, nullptr },           // 0x02
        { "contact_medical_service", 2, nullptr },        // 0x03
        { "panic_alert", 8, nullptr },                    // 0x04
    };
    static const EventsRangeList EmergencyAlarm = { { 0x00, EmergencyAlarmList_00 } };

    static const IdleValueHandler<NotificationType::Clock> ClockIdleHandler;
    static const EventDataList ClockList_00 = {
        { "state_idle", 4, &ClockIdleHandler },  // 0x00
        { "wake_up_alert", 2, nullptr },         // 0x01
        { "time_ended", 3, nullptr },            // 0x02
        { "time_remaining", 4, &TimeHandler },   // 0x03
    };
    static const EventsRangeList Clock = { { 0x00, ClockList_00 } };

    static const IdleValueHandler<NotificationType::Appliance> ApplianceIdleHandler;
    static const EventDataList ApplianceList_00 = {
        { "state_idle", 4, &ApplianceIdleHandler },           // 0x00
        { "program_started", 4, nullptr },                    // 0x01
        { "program_in_progress", 4, nullptr },                // 0x02
        { "program_completed", 4, nullptr },                  // 0x03
        { "replace_main_filter", 4, nullptr },                // 0x04
        { "failure_to_set_target_temperature", 4, nullptr },  // 0x05
        { "supplying_water", 4, nullptr },                    // 0x06
        { "water_supply_failure", 4, nullptr },               // 0x07
        { "boiling", 4, nullptr },                            // 0x08
        { "boiling_failure", 4, nullptr },                    // 0x09
        { "washing", 4, nullptr },                            // 0x0A
        { "washing_failure", 4, nullptr },                    // 0x0B
        { "rinsing", 4, nullptr },                            // 0x0C
        { "rinsing_failure", 4, nullptr },                    // 0x0D
        { "draining", 4, nullptr },                           // 0x0E
        { "draining_failure", 4, nullptr },                   // 0x0F
        { "spinning", 4, nullptr },                           // 0x10
        { "spinning_failure", 4, nullptr },                   // 0x11
        { "drying", 4, nullptr },                             // 0x12
        { "drying_failure", 4, nullptr },                     // 0x13
        { "fan_failure", 4, nullptr },                        // 0x14
        { "compressor_failure", 4, nullptr },                 // 0x15
    };
    static const EventsRangeList Appliance = { { 0x00, ApplianceList_00 } };

    static const IdleValueHandler<NotificationType::HomeHealth> HomeHealthIdleHandler;
    static const EventDataList HomeHealthList_00 = {
        { "state_idle", 4, &HomeHealthIdleHandler },                       // 0x00
        { "leaving_bed", 4, nullptr },                                     // 0x01
        { "sitting_on_bed", 4, nullptr },                                  // 0x02
        { "lying_on_bed", 4, nullptr },                                    // 0x03
        { "posture_changed", 4, nullptr },                                 // 0x04
        { "sitting_on_bed_edge", 4, nullptr },                             // 0x05
        { "volatile_organic_compound_level", 4, &PollutionLevelHandler },  // 0x06
        { "sleep_apnea_detected", 4, &BreathLevelHandler },                // 0x07
        { "sleep_stage_0_detected", 4, nullptr },                          // 0x08
        { "sleep_stage_1_detected", 4, nullptr },                          // 0x09
        { "sleep_stage_2_detected", 4, nullptr },                          // 0x0A
        { "sleep_stage_3_detected", 4, nullptr },                          // 0x0B
    };
    static const EventsRangeList HomeHealth = { { 0x00, HomeHealthList_00 } };

    static const IdleValueHandler<NotificationType::Siren> SirenIdleHandler;
    static const EventDataList SirenList_00 = {
        { "state_idle", 6, &SirenIdleHandler },  // 0x00
        { "siren_active", 6, nullptr },          // 0x01
    };
    static const EventsRangeList Siren = { { 0x00, SirenList_00 } };

    static const IdleValueHandler<NotificationType::WaterValve> WaterValveIdleHandler;
    static const EventDataList WaterValveList_00 = {
        { "state_idle", 7, &WaterValveIdleHandler },                   // 0x00
        { "valve_operation", 7, &OnOffHandler },                       // 0x01
        { "master_valve_operation", 7, &OnOffHandler },                // 0x02
        { "valve_short_circuit", 7, nullptr },                         // 0x03
        { "master_valve_short_circuit", 7, nullptr },                  // 0x04
        { "valve_current_alarm", 7, &ThresholdResultHandler },         // 0x05
        { "master_valve_current_alarm", 7, &ThresholdResultHandler },  // 0x06
    };
    static const EventsRangeList WaterValve = { { 0x00, WaterValveList_00 } };

    static const IdleValueHandler<NotificationType::WeatherAlarm> WeatherAlarmIdleHandler;
    static const EventDataList WheatherAlarmList_00 = {
        { "state_idle", 7, &WeatherAlarmIdleHandler },  // 0x00
        { "rain", 7, nullptr },                         // 0x01
        { "moisture", 7, nullptr },                     // 0x02
        { "freeze", 8, nullptr },                       // 0x03
    };
    static const EventsRangeList WeatherAlarm = { { 0x00, WheatherAlarmList_00 } };

    static const IdleValueHandler<NotificationType::Irrigation> IrrigationIdleHandler;
    static const EventDataList IrrigationList_00 = {
        { "state_idle", 7, &IrrigationIdleHandler },      // 0x00
        { "shedule_started", 7, &UintHandler },           // 0x01
        { "shedule_finished", 7, &UintHandler },          // 0x02
        { "valve_table_run_started", 7, &UintHandler },   // 0x03
        { "valve_table_run_finished", 7, &UintHandler },  // 0x04
        { "device_not_configured", 7, nullptr },          // 0x05
    };
    static const EventsRangeList Irrigation = { { 0x00, IrrigationList_00 } };

    static const IdleValueHandler<NotificationType::GasAlarm> GasAlarmIdleHandler;
    static const EventDataList GasAlarmList_00 = {
        { "state_idle", 7, &GasAlarmIdleHandler },             // 0x00
        { "combustible_gas_detected ", 7, &LocationHandler },  // 0x01
        { "combustible_gas_detected", 7, nullptr },            // 0x02
        { "toxic_gas_detected", 7, &LocationHandler },         // 0x03
        { "toxic_gas_detected", 7, nullptr },                  // 0x04
        { "gas_alarm_test", 7, nullptr },                      // 0x05
        { "replacement_required", 7, nullptr },                // 0x06
    };
    static const EventsRangeList GasAlarm = { { 0x00, GasAlarmList_00 } };

    static const IdleValueHandler<NotificationType::PestControl> PestControlIdleHandler;
    static const EventDataList PestControlList_00 = {
        { "state_idle", 8, &PestControlIdleHandler },    // 0x00
        { "trap_armed ", 8, &LocationHandler },          // 0x01
        { "trap_armed", 8, nullptr },                    // 0x02
        { "trap_rearm_required", 8, &LocationHandler },  // 0x03
        { "trap_rearm_required", 8, nullptr },           // 0x04
        { "pest_detected", 8, &LocationHandler },        // 0x05
        { "pest_detected", 8, nullptr },                 // 0x06
        { "pest_exterminated", 8, &LocationHandler },    // 0x07
        { "pest_exterminated", 8, nullptr },             // 0x08
    };
    static const EventsRangeList PestControl = { { 0x00, PestControlList_00 } };

    static const IdleValueHandler<NotificationType::LightSensor> LightSensorIdleHandler;
    static const EventDataList LightSensorList_00 = {
        { "state_idle", 8, &LightSensorIdleHandler },       // 0x00
        { "light_detected ", 8, nullptr },                  // 0x01
        { "light_color_transition_detected", 8, nullptr },  // 0x02
    };
    static const EventsRangeList LightSensor = { { 0x00, LightSensorList_00 } };

    static const IdleValueHandler<NotificationType::WaterQualityMonitoring> WaterQualityMonitoringIdleHandler;
    static const EventDataList WaterQualityMonitoringList_00 = {
        { "state_idle", 8, &WaterQualityMonitoringIdleHandler },          // 0x00
        { "chlorine_alarm ", 8, &WaterQualityThresholdHandler },          // 0x01
        { "acidity_alarm", 8, &PhHandler },                               // 0x02
        { "water_oxidation_alarm", 8, &WaterQualityThresholdHandler },    // 0x03
        { "chlorine_empty", 8, nullptr },                                 // 0x04
        { "acidity_empty", 8, nullptr },                                  // 0x05
        { "waterflow_measuring_station_shortage_detected", 8, nullptr },  // 0x06
        { "waterflow_clear_water_shortage_detected", 8, nullptr },        // 0x07
        { "disinfection_system_error_detected", 8, nullptr /*TODO*/ },    // 0x08
        { "filter_cleaning_ongoing", 8, nullptr /*TODO*/ },               // 0x09
        { "heating_cleaning_ongoing", 8, nullptr },                       // 0x0A
        { "filter_pump_cleaning_ongoing", 8, nullptr },                   // 0x0B
        { "fresh_water_cleaning_ongoing", 8, nullptr },                   // 0x0C
        { "dry_protection_operation_active", 8, nullptr },                // 0x0D
        { "water_tank_empty", 8, nullptr },                               // 0x0E
        { "water_tank_level_unknown", 8, nullptr },                       // 0x0F
        { "water_tank_full", 8, nullptr },                                // 0x10
        { "collective_disorder", 8, nullptr },                            // 0x11
    };
    static const EventsRangeList WaterQualityMonitoring = { { 0x00, WaterQualityMonitoringList_00 } };

    static const IdleValueHandler<NotificationType::HomeMonitoring> HomeMonitoringIdleHandler;
    static const EventDataList HomeMonitoringList_00 = {
        { "state_idle", 8, &HomeMonitoringIdleHandler },  // 0x00
        { "home_occupied ", 8, &LocationHandler },        // 0x01
        { "home_occupied", 8, nullptr },                  // 0x02
    };
    static const EventsRangeList HomeMonitoring = { { 0x00, HomeMonitoringList_00 } };

    static const std::initializer_list<const EventsRangeList> NotificationEventsDataList_01 = {
        SmokeAlarm,              // 0x01
        CoAlarm,                 // 0x02
        Co2Alarm,                // 0x03
        HeatAlarm,               // 0x04
        WaterAlarm,              // 0x05
        AccessControl,           // 0x06
        HomeSecurity,            // 0x07
        PowerManagement,         // 0x08
        System,                  // 0x09
        EmergencyAlarm,          // 0x0A
        Clock,                   // 0x0B
        Appliance,               // 0x0C
        HomeHealth,              // 0x0D
        Siren,                   // 0x0E
        WaterValve,              // 0x0F
        WeatherAlarm,            // 0x10
        Irrigation,              // 0x11
        GasAlarm,                // 0x12
        PestControl,             // 0x13
        LightSensor,             // 0x14
        WaterQualityMonitoring,  // 0x15
        HomeMonitoring,          // 0x16
    };

    static const NotificationEventsList NotificationEventsData = { { NotificationType::SmokeAlarm,
                                                                     NotificationEventsDataList_01 } };

    return NotificationEventsData;
}

const RangedList<NotificationType, const char*>& getNotificationNamesList() {
    static const StringList Names_01 = {
        "smoke_alarm",               // 0x01
        "co_alarm",                  // 0x02
        "co2_alarm",                 // 0x03
        "heat_alarm",                // 0x04
        "water_alarm",               // 0x05
        "access_control",            // 0x06
        "home_security",             // 0x07
        "power_management",          // 0x08
        "system",                    // 0x09
        "emergency_alarm",           // 0x0A
        "clock",                     // 0x0B
        "appliance",                 // 0x0C
        "home_health",               // 0x0D
        "siren",                     // 0x0E
        "water_valve",               // 0x0F
        "weather_alarm",             // 0x10
        "irrigation",                // 0x11
        "gas_alarm",                 // 0x12
        "pest_control",              // 0x13
        "light_sensor",              // 0x14
        "water_quality_monitoring",  // 0x15
        "home_monitoring",           // 0x16
    };
    static const StringList Names_FF = {
        "request_pending_notification"  // 0xFF
    };
    static const RangedList<NotificationType, const char*> Names = {
        { NotificationType::SmokeAlarm, Names_01 }, { NotificationType::RequestPendingNotification, Names_FF }
    };

    return Names;
}

}  // namespace

const NotificationEventData* getNotificationEventData( NotificationType notificationType,
                                                       NotificationEvent eventType ) {
    const auto events = findInRangedList( getNotificationEventsList(), notificationType );

    if( !events ) {
        return nullptr;
    }

    const auto result = findInRangedList( *events, eventType );

    return result && result->m_name ? result : nullptr;
}

const char* getNotificationName( NotificationType notificationType ) {
    const auto name = findInRangedList( getNotificationNamesList(), notificationType );

    return name ? *name : nullptr;
}

NotificationTypeOpt getNotificationType( const char* notificationName ) {
    return findIndexInRangedList( getNotificationNamesList(), [notificationName]( const char* currentName ) {
        return strcmp( notificationName, currentName ) == 0;
    } );
}

NotificationEventOpt getNotificationEvent( NotificationType notificationType, const char* eventName ) {
    const auto events = findInRangedList( getNotificationEventsList(), notificationType );

    if( !events ) {
        return {};
    }

    return findIndexInRangedList( *events, [eventName]( const NotificationEventData& eventData ) {
        return strcmp( eventName, eventData.m_name ) == 0;
    } );
}

}  // namespace znet
