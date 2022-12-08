//////////////////////////////////////////////////////////////////////////////
#include "macros.h"
#include "array_bin_search.h"
#include "znet_lib_cc.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
typedef struct _CC_reports_item {
    uint8_t _id;
    uint8_t _cmd;
    CC_Report _Report;
    uint8_t _type;
} CC_reports_item_t;
typedef CC_reports_item_t* CC_reports_item;

//----------------------------------------------------------------------------
/* clang-format off */
CC_reports_item_t cc_reports[] =
{
//  {         _id                                      ,         _cmd                                         ,  _Report                                            , _type         },
    { /*0x20*/COMMAND_CLASS_BASIC                      , /*0x01*/BASIC_SET                                    , &znet_cc_basic_receive_set                              , MESSAGE_TYPE_SET_GET   },
    { /*0x20*/COMMAND_CLASS_BASIC                      , /*0x02*/BASIC_GET                                    , &znet_cc_basic_receive                              , MESSAGE_TYPE_SET_GET   },
    { /*0x20*/COMMAND_CLASS_BASIC                      , /*0x03*/BASIC_REPORT                                 , &znet_cc_basic_report                               , MESSAGE_TYPE_REPORT    },

    { /*0x25*/COMMAND_CLASS_SWITCH_BINARY              , /*0x03*/SWITCH_BINARY_REPORT                         , &znet_cc_switch_binary_report                       , MESSAGE_TYPE_REPORT    },

    { /*0x26*/COMMAND_CLASS_SWITCH_MULTILEVEL          , /*0x03*/SWITCH_MULTILEVEL_REPORT                     , &znet_cc_switch_multilevel_report                   , MESSAGE_TYPE_REPORT    },
    { /*0x26*/COMMAND_CLASS_SWITCH_MULTILEVEL          , /*0x07*/SWITCH_MULTILEVEL_SUPPORTED_REPORT_V4        , &znet_cc_switch_multilevel_supported_report         , MESSAGE_TYPE_REPORT    },

//    { /*0x27*/COMMAND_CLASS_SWITCH_ALL                 , /*0x03*/SWITCH_ALL_REPORT                            , &znet_cc_switch_all_report                          , MESSAGE_TYPE_REPORT    },

    { /*0x2B*/COMMAND_CLASS_SCENE_ACTIVATION           , /*0x03*/SCENE_ACTIVATION_SET                         , &znet_cc_scene_activation_receive_set               , MESSAGE_TYPE_SET_GET    },

    { /*0x2C*/COMMAND_CLASS_SCENE_ACTUATOR_CONF        , /*0x03*/SCENE_ACTUATOR_CONF_REPORT                   , &znet_cc_scene_actuator_conf_report                 , MESSAGE_TYPE_REPORT    },
    { /*0x2D*/COMMAND_CLASS_SCENE_CONTROLLER_CONF      , /*0x03*/SCENE_CONTROLLER_CONF_REPORT                 , &znet_cc_scene_controller_conf_report               , MESSAGE_TYPE_REPORT    },

    { /*0x30*/COMMAND_CLASS_SENSOR_BINARY              , /*0x03*/SENSOR_BINARY_REPORT                         , &znet_cc_sensor_binary_report                       , MESSAGE_TYPE_REPORT    },
    { /*0x30*/COMMAND_CLASS_SENSOR_BINARY              , /*0x04*/SENSOR_BINARY_SUPPORTED_SENSOR_REPORT_V2     , &znet_cc_sensor_binary_supported_report             , MESSAGE_TYPE_REPORT    },

    { /*0x31*/COMMAND_CLASS_SENSOR_MULTILEVEL          , /*0x02*/SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V11, &znet_cc_sensor_multilevel_supported_sensor_report  , MESSAGE_TYPE_REPORT    },
    { /*0x31*/COMMAND_CLASS_SENSOR_MULTILEVEL          , /*0x05*/SENSOR_MULTILEVEL_REPORT                     , &znet_cc_sensor_multilevel_report                   , MESSAGE_TYPE_REPORT    },
    { /*0x31*/COMMAND_CLASS_SENSOR_MULTILEVEL          , /*0x06*/SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V11 , &znet_cc_sensor_multilevel_supported_scale_report   , MESSAGE_TYPE_REPORT    },

    { /*0x32*/COMMAND_CLASS_METER                      , /*0x02*/METER_REPORT                                 , &znet_cc_meter_report                               , MESSAGE_TYPE_REPORT    },
    { /*0x32*/COMMAND_CLASS_METER                      , /*0x04*/METER_SUPPORTED_REPORT_V5                    , &znet_cc_meter_supported_report                     , MESSAGE_TYPE_REPORT    },

    { /*0x33*/COMMAND_CLASS_SWITCH_COLOR               , /*0x02*/SWITCH_COLOR_SUPPORTED_REPORT                , &znet_cc_color_switch_supported_report              , MESSAGE_TYPE_REPORT    },
    { /*0x33*/COMMAND_CLASS_SWITCH_COLOR               , /*0x04*/SWITCH_COLOR_REPORT                          , &znet_cc_color_switch_report                        , MESSAGE_TYPE_REPORT    },

    { /*0x40*/COMMAND_CLASS_THERMOSTAT_MODE            , /*0x03*/THERMOSTAT_MODE_REPORT                       , &znet_cc_thermostat_mode_report                     , MESSAGE_TYPE_REPORT    },
    { /*0x40*/COMMAND_CLASS_THERMOSTAT_MODE            , /*0x05*/THERMOSTAT_MODE_SUPPORTED_REPORT             , &znet_cc_thermostat_mode_supported_report           , MESSAGE_TYPE_REPORT    },

    { /*0x42*/COMMAND_CLASS_THERMOSTAT_OPERATING_STATE , /*0x03*/THERMOSTAT_OPERATING_STATE_REPORT            , &znet_cc_thermostat_operating_state_report          , MESSAGE_TYPE_REPORT    },
    { /*0x42*/COMMAND_CLASS_THERMOSTAT_OPERATING_STATE , /*0x04*/THERMOSTAT_OPERATING_LOGGING_SUPPORTED_REPORT_V2, &znet_cc_thermostat_operating_state_logging_supported_report , MESSAGE_TYPE_REPORT},
    { /*0x42*/COMMAND_CLASS_THERMOSTAT_OPERATING_STATE , /*0x06*/THERMOSTAT_OPERATING_STATE_LOGGING_REPORT_V2 , &znet_cc_thermostat_operating_state_logging_report  , MESSAGE_TYPE_REPORT    },

    { /*0x43*/COMMAND_CLASS_THERMOSTAT_SETPOINT        , /*0x03*/THERMOSTAT_SETPOINT_REPORT                   , &znet_cc_thermostat_setpoint_report                 , MESSAGE_TYPE_REPORT    },
    { /*0x43*/COMMAND_CLASS_THERMOSTAT_SETPOINT        , /*0x05*/THERMOSTAT_SETPOINT_SUPPORTED_REPORT         , &znet_cc_thermostat_setpoint_supported_report       , MESSAGE_TYPE_REPORT    },
    { /*0x43*/COMMAND_CLASS_THERMOSTAT_SETPOINT        , /*0x0A*/THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_V3   , &znet_cc_thermostat_setpoint_capabilities_report    , MESSAGE_TYPE_REPORT    },

    { /*0x44*/COMMAND_CLASS_THERMOSTAT_FAN_MODE        , /*0x03*/THERMOSTAT_FAN_MODE_REPORT                   , &znet_cc_thermostat_fan_mode_report                 , MESSAGE_TYPE_REPORT    },
    { /*0x44*/COMMAND_CLASS_THERMOSTAT_FAN_MODE        , /*0x05*/THERMOSTAT_FAN_MODE_SUPPORTED_REPORT         , &znet_cc_thermostat_fan_mode_supported_report       , MESSAGE_TYPE_REPORT    },

    { /*0x45*/COMMAND_CLASS_THERMOSTAT_FAN_STATE       , /*0x03*/THERMOSTAT_FAN_STATE_REPORT                  , &znet_cc_thermostat_fan_state_report                , MESSAGE_TYPE_REPORT    },

    { /*0x47*/COMMAND_CLASS_THERMOSTAT_SETBACK         , /*0x03*/THERMOSTAT_SETBACK_REPORT                    , &znet_cc_thermostat_setback_report                  , MESSAGE_TYPE_REPORT    },

//    { /*0x4C*/COMMAND_CLASS_DOOR_LOCK_LOGGING          , /*0x02*/DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT   , &znet_cc_door_lock_logging_records_supported_report , MESSAGE_TYPE_REPORT    },
//    { /*0x4C*/COMMAND_CLASS_DOOR_LOCK_LOGGING          , /*0x04*/RECORD_REPORT                                , &znet_cc_record_report                              , MESSAGE_TYPE_REPORT    },

    { /*0x4E*/COMMAND_CLASS_SCHEDULE_ENTRY_LOCK        , /*0x05*/SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT          , &znet_cc_schedule_entry_lock_week_day_report        , MESSAGE_TYPE_REPORT    },
    { /*0x4E*/COMMAND_CLASS_SCHEDULE_ENTRY_LOCK        , /*0x08*/SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT          , &znet_cc_schedule_entry_lock_year_day_report        , MESSAGE_TYPE_REPORT    },
    { /*0x4E*/COMMAND_CLASS_SCHEDULE_ENTRY_LOCK        , /*0x0A*/SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT         , &znet_cc_schedule_entry_type_supported_report       , MESSAGE_TYPE_REPORT    },
    { /*0x4E*/COMMAND_CLASS_SCHEDULE_ENTRY_LOCK        , /*0x0C*/SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V3    , &znet_cc_schedule_entry_lock_time_offset_report     , MESSAGE_TYPE_REPORT    },
    { /*0x4E*/COMMAND_CLASS_SCHEDULE_ENTRY_LOCK        , /*0x0F*/SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_REPORT_V3, &znet_cc_schedule_entry_lock_daily_repeating_report , MESSAGE_TYPE_REPORT    },

    { /*0x55*/COMMAND_CLASS_TRANSPORT_SERVICE          , /*0xC0*/COMMAND_FIRST_SEGMENT_V2                     , &znet_cc_transport_receive                          , MESSAGE_TYPE_TRANSPORT },
    { /*0x55*/COMMAND_CLASS_TRANSPORT_SERVICE          , /*0xC8*/COMMAND_SEGMENT_REQUEST_V2                   , &znet_cc_transport_receive                          , MESSAGE_TYPE_TRANSPORT },
    { /*0x55*/COMMAND_CLASS_TRANSPORT_SERVICE          , /*0xE0*/COMMAND_SUBSEQUENT_SEGMENT_V2                , &znet_cc_transport_receive                          , MESSAGE_TYPE_TRANSPORT },
    { /*0x55*/COMMAND_CLASS_TRANSPORT_SERVICE          , /*0xE8*/COMMAND_SEGMENT_COMPLETE_V2                  , &znet_cc_transport_receive                          , MESSAGE_TYPE_TRANSPORT },
    { /*0x55*/COMMAND_CLASS_TRANSPORT_SERVICE          , /*0xF0*/COMMAND_SEGMENT_WAIT_V2                      , &znet_cc_transport_receive                          , MESSAGE_TYPE_TRANSPORT },

    { /*0x56*/COMMAND_CLASS_CRC_16_ENCAP               , /*0x01*/CRC_16_ENCAP                                 , &znet_cc_crc16_encrypt                              , MESSAGE_TYPE_TRANSPORT },

    { /*0x59*/COMMAND_CLASS_ASSOCIATION_GRP_INFO       , /*0x01*/ASSOCIATION_GROUP_NAME_GET_V3                , &znet_cc_association_group_info_receive             , MESSAGE_TYPE_SET_GET   },
    { /*0x59*/COMMAND_CLASS_ASSOCIATION_GRP_INFO       , /*0x02*/ASSOCIATION_GROUP_NAME_REPORT                , &znet_cc_association_group_name_report              , MESSAGE_TYPE_REPORT    },
    { /*0x59*/COMMAND_CLASS_ASSOCIATION_GRP_INFO       , /*0x03*/ASSOCIATION_GROUP_INFO_GET_V3                , &znet_cc_association_group_info_receive             , MESSAGE_TYPE_SET_GET   },
    { /*0x59*/COMMAND_CLASS_ASSOCIATION_GRP_INFO       , /*0x04*/ASSOCIATION_GROUP_INFO_REPORT                , &znet_cc_association_group_info_report              , MESSAGE_TYPE_REPORT    },
    { /*0x59*/COMMAND_CLASS_ASSOCIATION_GRP_INFO       , /*0x05*/ASSOCIATION_GROUP_COMMAND_LIST_GET_V3        , &znet_cc_association_group_info_receive             , MESSAGE_TYPE_SET_GET   },
    { /*0x59*/COMMAND_CLASS_ASSOCIATION_GRP_INFO       , /*0x06*/ASSOCIATION_GROUP_COMMAND_LIST_REPORT        , &znet_cc_association_group_command_list_report      , MESSAGE_TYPE_REPORT    },

    { /*0x5A*/COMMAND_CLASS_DEVICE_RESET_LOCALLY       , /*0x01*/DEVICE_RESET_LOCALLY_NOTIFICATION            , &znet_cc_device_reset_localy_report_lib             , MESSAGE_TYPE_SET_GET   },

    { /*0x5B*/COMMAND_CLASS_CENTRAL_SCENE              , /*0x02*/CENTRAL_SCENE_SUPPORTED_REPORT               , &znet_cc_central_scene_supported_report             , MESSAGE_TYPE_REPORT    },
    { /*0x5B*/COMMAND_CLASS_CENTRAL_SCENE              , /*0x03*/CENTRAL_SCENE_NOTIFICATION                   , &znet_cc_central_scene_notification                 , MESSAGE_TYPE_REPORT    },
    { /*0x5B*/COMMAND_CLASS_CENTRAL_SCENE              , /*0x06*/CENTRAL_SCENE_CONFIGURATION_REPORT           , &znet_cc_central_scene_configuration_report         , MESSAGE_TYPE_REPORT    },

    { /*0x5D*/COMMAND_CLASS_ANTITHEFT                  , /*0x03*/ANTITHEFT_REPORT                             , &znet_cc_anti_theft_report                          , MESSAGE_TYPE_REPORT    },

    { /*0x5E*/COMMAND_CLASS_ZWAVEPLUS_INFO             , /*0x01*/ZWAVEPLUS_INFO_GET                           , &znet_cc_zwave_plus_info_request                    , MESSAGE_TYPE_SET_GET   },
    { /*0x5E*/COMMAND_CLASS_ZWAVEPLUS_INFO             , /*0x02*/ZWAVEPLUS_INFO_REPORT                        , &znet_cc_zwave_plus_info_report                     , MESSAGE_TYPE_REPORT    },

    { /*0x60*/COMMAND_CLASS_MULTI_CHANNEL_V4           , /*0x05*/MULTI_INSTANCE_REPORT                        , &znet_cc_multichannel_instance_report               , MESSAGE_TYPE_REPORT    },
    { /*0x60*/COMMAND_CLASS_MULTI_CHANNEL_V4           , /*0x08*/MULTI_CHANNEL_END_POINT_REPORT_V4            , &znet_cc_multichannel_endpoint_report               , MESSAGE_TYPE_REPORT    },
    { /*0x60*/COMMAND_CLASS_MULTI_CHANNEL_V4           , /*0x0A*/MULTI_CHANNEL_CAPABILITY_REPORT_V4           , &znet_cc_multichannel_capability_report             , MESSAGE_TYPE_REPORT    },
    { /*0x60*/COMMAND_CLASS_MULTI_CHANNEL_V4           , /*0x0C*/MULTI_CHANNEL_END_POINT_FIND_REPORT_V4       , &znet_cc_multichannel_endpoint_find_report          , MESSAGE_TYPE_REPORT    },
    { /*0x60*/COMMAND_CLASS_MULTI_CHANNEL_V4           , /*0x0D*/MULTI_CHANNEL_CMD_ENCAP_V4                   , &znet_cc_multichannel_recieve                       , MESSAGE_TYPE_REPORT    },
    { /*0x60*/COMMAND_CLASS_MULTI_CHANNEL_V4           , /*0x0F*/MULTI_CHANNEL_AGGREGATED_MEMBERS_REPORT_V4   , &znet_cc_multichannel_aggregated_members_report     , MESSAGE_TYPE_REPORT    },

    { /*0x62*/COMMAND_CLASS_DOOR_LOCK                  , /*0x03*/DOOR_LOCK_OPERATION_REPORT                   , &znet_cc_door_lock_operation_report                 , MESSAGE_TYPE_REPORT    },
    { /*0x62*/COMMAND_CLASS_DOOR_LOCK                  , /*0x06*/DOOR_LOCK_CONFIGURATION_REPORT               , &znet_cc_door_lock_configuration_report             , MESSAGE_TYPE_REPORT    },
    { /*0x62*/COMMAND_CLASS_DOOR_LOCK                  , /*0x08*/DOOR_LOCK_CAPABILITIES_REPORT                , &znet_cc_door_lock_capabilities_report              , MESSAGE_TYPE_REPORT    },

    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x03*/USER_CODE_REPORT                             , &znet_cc_user_code_report                           , MESSAGE_TYPE_REPORT    },
    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x05*/USERS_NUMBER_REPORT                          , &znet_cc_users_number_report                        , MESSAGE_TYPE_REPORT    },
    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x07*/USER_CODE_CAPABILITIES_REPORT                , &znet_cc_user_code_capabilities_report              , MESSAGE_TYPE_REPORT    },
    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x09*/USER_CODE_KEYPAD_MODE_REPORT                 , &znet_cc_user_code_keypad_mode_report               , MESSAGE_TYPE_REPORT    },
    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x0C*/EXTENDED_USER_CODE_REPORT                    , &znet_cc_extended_user_code_report                  , MESSAGE_TYPE_REPORT    },
    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x10*/MASTER_CODE_REPORT                           , &znet_cc_master_code_report                         , MESSAGE_TYPE_REPORT    },
    { /*0x63*/COMMAND_CLASS_USER_CODE                  , /*0x12*/USER_CODE_CHECKSUM_REPORT                    , &znet_cc_user_code_checksum_report                  , MESSAGE_TYPE_REPORT    },

    { /*0x66*/COMMAND_CLASS_BARRIER_OPERATOR           , /*0x03*/BARRIER_OPERATOR_REPORT                      , &znet_cc_barrier_operator_report                    , MESSAGE_TYPE_REPORT    },
    { /*0x66*/COMMAND_CLASS_BARRIER_OPERATOR           , /*0x05*/BARRIER_OPERATOR_SIGNAL_SUPPORTED_REPORT     , &znet_cc_barrier_operator_supported_signals_report  , MESSAGE_TYPE_REPORT    },
    { /*0x66*/COMMAND_CLASS_BARRIER_OPERATOR           , /*0x08*/BARRIER_OPERATOR_SIGNAL_REPORT               , &znet_cc_barrier_operator_signal_report             , MESSAGE_TYPE_REPORT    },

    { /*0x6C*/COMMAND_CLASS_SUPERVISION                , /*0x01*/SUPERVISION_GET                              , &znet_cc_supervision_receive                        , MESSAGE_TYPE_SET_GET   },
    { /*0x6C*/COMMAND_CLASS_SUPERVISION                , /*0x02*/SUPERVISION_REPORT                           , &znet_cc_supervision_report                         , MESSAGE_TYPE_REPORT    },

    { /*0x6F*/COMMAND_CLASS_ENTRY_CONTROL              , /*0x01*/ENTRY_CONTROL_NOTIFICATION                   , &znet_cc_entry_control_notification                 , MESSAGE_TYPE_REPORT    },
    { /*0x6F*/COMMAND_CLASS_ENTRY_CONTROL              , /*0x03*/ENTRY_CONTROL_KEY_SUPPORTED_REPORT           , &znet_cc_entry_control_key_supported_report         , MESSAGE_TYPE_REPORT    },
    { /*0x6F*/COMMAND_CLASS_ENTRY_CONTROL              , /*0x05*/ENTRY_CONTROL_EVENT_SUPPORTED_REPORT         , &znet_cc_entry_control_event_supported_report       , MESSAGE_TYPE_REPORT    },
    { /*0x6F*/COMMAND_CLASS_ENTRY_CONTROL              , /*0x08*/ENTRY_CONTROL_CONFIGURATION_REPORT           , &znet_cc_entry_control_configuration_report         , MESSAGE_TYPE_REPORT    },

    { /*0x70*/COMMAND_CLASS_CONFIGURATION              , /*0x06*/CONFIGURATION_REPORT                         , &znet_cc_configuration_report                       , MESSAGE_TYPE_REPORT    },
    { /*0x70*/COMMAND_CLASS_CONFIGURATION              , /*0x09*/CONFIGURATION_BULK_REPORT_V4                 , &znet_cc_configuration_bulk_report                  , MESSAGE_TYPE_REPORT    },
    { /*0x70*/COMMAND_CLASS_CONFIGURATION              , /*0x0B*/CONFIGURATION_NAME_REPORT_V4                 , &znet_cc_configuration_name_report                  , MESSAGE_TYPE_REPORT    },
    { /*0x70*/COMMAND_CLASS_CONFIGURATION              , /*0x0D*/CONFIGURATION_INFO_REPORT_V4                 , &znet_cc_configuration_info_report                  , MESSAGE_TYPE_REPORT    },
    { /*0x70*/COMMAND_CLASS_CONFIGURATION              , /*0x0F*/CONFIGURATION_PROPERTIES_REPORT_V4           , &znet_cc_configuration_properties_report            , MESSAGE_TYPE_REPORT    },

//    { /*0x71*/COMMAND_CLASS_ALARM                      , /*0x05*/ALARM_REPORT                                 , &znet_cc_alarm_report                                        },

    { /*0x71*/COMMAND_CLASS_NOTIFICATION_V8            , /*0x02*/EVENT_SUPPORTED_REPORT_V8                    , &znet_cc_notification_event_supported_report        , MESSAGE_TYPE_REPORT    },
    { /*0x71*/COMMAND_CLASS_NOTIFICATION_V8            , /*0x05*/NOTIFICATION_REPORT_V8                       , &znet_cc_notification_report                        , MESSAGE_TYPE_REPORT    },
    { /*0x71*/COMMAND_CLASS_NOTIFICATION_V8            , /*0x08*/NOTIFICATION_SUPPORTED_REPORT_V8             , &znet_cc_notification_supported_report              , MESSAGE_TYPE_REPORT    },

    { /*0x72*/COMMAND_CLASS_MANUFACTURER_SPECIFIC      , /*0x04*/MANUFACTURER_SPECIFIC_GET_V2                 , &znet_cc_manufacturer_specific_requested            , MESSAGE_TYPE_SET_GET   },
    { /*0x72*/COMMAND_CLASS_MANUFACTURER_SPECIFIC      , /*0x05*/MANUFACTURER_SPECIFIC_REPORT                 , &znet_cc_manufacturer_specific_report               , MESSAGE_TYPE_REPORT    },
    { /*0x72*/COMMAND_CLASS_MANUFACTURER_SPECIFIC      , /*0x07*/DEVICE_SPECIFIC_REPORT_V2                    , &znet_cc_device_specific_report                     , MESSAGE_TYPE_REPORT    },


    { /*0x73*/COMMAND_CLASS_POWERLEVEL                 , /*0x01*/POWERLEVEL_SET                               , &znet_cc_powerlevel_receive                         , MESSAGE_TYPE_SET_GET   },
    { /*0x73*/COMMAND_CLASS_POWERLEVEL                 , /*0x02*/POWERLEVEL_GET                               , &znet_cc_powerlevel_receive                         , MESSAGE_TYPE_SET_GET   },
    { /*0x73*/COMMAND_CLASS_POWERLEVEL                 , /*0x03*/POWERLEVEL_REPORT                            , &znet_cc_powerlevel_report                          , MESSAGE_TYPE_REPORT    },
    { /*0x73*/COMMAND_CLASS_POWERLEVEL                 , /*0x04*/POWERLEVEL_TEST_NODE_SET                     , &znet_cc_powerlevel_receive                         , MESSAGE_TYPE_SET_GET   },
    { /*0x73*/COMMAND_CLASS_POWERLEVEL                 , /*0x05*/POWERLEVEL_TEST_NODE_GET                     , &znet_cc_powerlevel_receive                         , MESSAGE_TYPE_SET_GET   },
    { /*0x73*/COMMAND_CLASS_POWERLEVEL                 , /*0x06*/POWERLEVEL_TEST_NODE_REPORT                  , &znet_cc_powerlevel_test_node_report                , MESSAGE_TYPE_REPORT    },

    { /*0x74*/COMMAND_CLASS_INCLUSION_CONTROLLER       , /*0x01*/INITIATE                                     , &znet_cc_inclusion_receive                          , MESSAGE_TYPE_SET_GET   },
    { /*0x74*/COMMAND_CLASS_INCLUSION_CONTROLLER       , /*0x02*/COMPLETE                                     , &znet_cc_inclusion_receive                          , MESSAGE_TYPE_SET_GET   },

//    { /*0x75*/COMMAND_CLASS_PROTECTION                 , /*0x03*/PROTECTION_REPORT                            , &znet_cc_protection_report                          , MESSAGE_TYPE_REPORT    },
//    { /*0x75*/COMMAND_CLASS_PROTECTION                 , /*0x05*/PROTECTION_SUPPORTED_REPORT_V2               , &znet_cc_protection_supported_report                , MESSAGE_TYPE_REPORT    },
//    { /*0x75*/COMMAND_CLASS_PROTECTION                 , /*0x08*/PROTECTION_EC_REPORT_V2                      , &znet_cc_protection_ec_report                       , MESSAGE_TYPE_REPORT    },
//    { /*0x75*/COMMAND_CLASS_PROTECTION                 , /*0x0B*/PROTECTION_TIMEOUT_REPORT_V2                 , &znet_cc_protection_timeout_report                  , MESSAGE_TYPE_REPORT    },

//    { /*0x77*/COMMAND_CLASS_NODE_NAMING                , /*0x03*/NODE_NAMING_NODE_NAME_REPORT                 , &znet_cc_node_naming_name_report                    , MESSAGE_TYPE_REPORT    },
//    { /*0x77*/COMMAND_CLASS_NODE_NAMING                , /*0x06*/NODE_NAMING_NODE_LOCATION_REPORT             , &znet_cc_node_naming_location_report                , MESSAGE_TYPE_REPORT    },

    { /*0x78*/COMMAND_CLASS_NODE_PROVISIONING          , /*0x01*/NODE_PROVISION_SET                           , &znet_cc_node_provisioning_set_received             , MESSAGE_TYPE_SET_GET   },
    { /*0x78*/COMMAND_CLASS_NODE_PROVISIONING          , /*0x02*/NODE_PROVISION_DELETE                        , &znet_cc_node_provisioning_delete_received          , MESSAGE_TYPE_SET_GET   },
    { /*0x78*/COMMAND_CLASS_NODE_PROVISIONING          , /*0x03*/NODE_PROVISIONING_LIST_ITERATION_GET         , &znet_cc_node_provisioning_list_iteration_get_received , MESSAGE_TYPE_SET_GET   },
    { /*0x78*/COMMAND_CLASS_NODE_PROVISIONING          , /*0x05*/NODE_PROVISION_GET                           , &znet_cc_node_provisioning_get_received             , MESSAGE_TYPE_SET_GET   },

    { /*0x79*/COMMAND_CLASS_SOUND_SWITCH               , /*0x02*/SOUND_SWITCH_TONES_NUMBER_REPORT             , &znet_cc_sound_switch_tones_number_report           , MESSAGE_TYPE_REPORT    },
    { /*0x79*/COMMAND_CLASS_SOUND_SWITCH               , /*0x04*/SOUND_SWITCH_TONE_INFO_REPORT                , &znet_cc_sound_switch_tone_info_report              , MESSAGE_TYPE_REPORT    },
    { /*0x79*/COMMAND_CLASS_SOUND_SWITCH               , /*0x07*/SOUND_SWITCH_CONFIGURATION_REPORT            , &znet_cc_sound_switch_configuration_report          , MESSAGE_TYPE_REPORT    },
    { /*0x79*/COMMAND_CLASS_SOUND_SWITCH               , /*0x0A*/SOUND_SWITCH_TONE_PLAY_REPORT                , &znet_cc_sound_switch_tone_play_report              , MESSAGE_TYPE_REPORT    },

    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x01*/FIRMWARE_MD_GET                              , &znet_cc_firmware_md_requested                      , MESSAGE_TYPE_SET_GET   },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x02*/FIRMWARE_MD_REPORT                           , &znet_cc_firmware_md_received                       , MESSAGE_TYPE_REPORT    },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x03*/FIRMWARE_UPDATE_MD_REQUEST_GET               , &znet_cc_firmware_update_md_request_received        , MESSAGE_TYPE_SET_GET   },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x04*/FIRMWARE_UPDATE_MD_REQUEST_REPORT            , &znet_cc_firmware_update_md_request_report_received , MESSAGE_TYPE_REPORT    },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x05*/FIRMWARE_UPDATE_MD_GET                       , &znet_cc_firmware_update_md_requested               , MESSAGE_TYPE_SET_GET   },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x06*/FIRMWARE_UPDATE_MD_REPORT                    , &znet_cc_firmware_update_md_received                , MESSAGE_TYPE_REPORT    },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x07*/FIRMWARE_UPDATE_MD_STATUS_REPORT             , &znet_cc_firmware_update_md_status_received         , MESSAGE_TYPE_REPORT    },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x09*/FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT     , &znet_cc_firmware_update_md_activation_status_received, MESSAGE_TYPE_REPORT  },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x0A*/FIRMWARE_UPDATE_MD_PREPARE_GET               , &znet_cc_firmware_update_md_prepare_requested       , MESSAGE_TYPE_SET_GET    },
    { /*0x7A*/COMMAND_CLASS_FIRMWARE_UPDATE_MD         , /*0x0B*/FIRMWARE_UPDATE_MD_PREPARE_REPORT            , &znet_cc_firmware_update_md_prepare_report_received , MESSAGE_TYPE_REPORT    },

    { /*0x7E*/COMMAND_CLASS_ANTITHEFT_UNLOCK           , /*0x02*/ANTITHEFT_UNLOCK_STATE_REPORT                , &znet_cc_anti_theft_unlock_state_report                   , MESSAGE_TYPE_REPORT    },

    { /*0x80*/COMMAND_CLASS_BATTERY                    , /*0x03*/BATTERY_REPORT                               , &znet_cc_battery_report                             , MESSAGE_TYPE_REPORT    },

    { /*0x81*/COMMAND_CLASS_CLOCK                      , /*0x06*/CLOCK_REPORT                                 , &znet_cc_clock_report                               , MESSAGE_TYPE_REPORT    },

    { /*0x82*/COMMAND_CLASS_HAIL                       , /*0x01*/HAIL                                         , &znet_cc_hail_report                                , MESSAGE_TYPE_REPORT    },

    { /*0x84*/COMMAND_CLASS_WAKE_UP                    , /*0x06*/WAKE_UP_INTERVAL_REPORT                      , &znet_cc_wakeup_report                              , MESSAGE_TYPE_REPORT    },
    { /*0x84*/COMMAND_CLASS_WAKE_UP                    , /*0x07*/WAKE_UP_NOTIFICATION                         , &znet_cc_wakeup_notification_lib                    , MESSAGE_TYPE_REPORT    },
    { /*0x84*/COMMAND_CLASS_WAKE_UP                    , /*0x09*/WAKE_UP_INTERVAL_CAPABILITIES_GET_V2         , &znet_cc_wakeup_receive                             , MESSAGE_TYPE_SET_GET   },
    { /*0x84*/COMMAND_CLASS_WAKE_UP                    , /*0x0A*/WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2      , &znet_cc_wakeup_capabilities_report                 , MESSAGE_TYPE_REPORT    },

    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x01*/ASSOCIATION_SET                              , &znet_cc_association_receive                        , MESSAGE_TYPE_SET_GET   },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x02*/ASSOCIATION_GET                              , &znet_cc_association_receive                        , MESSAGE_TYPE_SET_GET   },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x03*/ASSOCIATION_REPORT                           , &znet_cc_association_report                         , MESSAGE_TYPE_REPORT    },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x04*/ASSOCIATION_REMOVE_V2                        , &znet_cc_association_receive                        , MESSAGE_TYPE_SET_GET   },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x05*/ASSOCIATION_GROUPINGS_GET_V2                 , &znet_cc_association_receive                        , MESSAGE_TYPE_SET_GET   },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x06*/ASSOCIATION_GROUPINGS_REPORT                 , &znet_cc_association_groupings_report               , MESSAGE_TYPE_REPORT    },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x0B*/ASSOCIATION_SPECIFIC_GROUP_GET_V2            , &znet_cc_association_receive                        , MESSAGE_TYPE_SET_GET   },
    { /*0x85*/COMMAND_CLASS_ASSOCIATION                , /*0x0C*/ASSOCIATION_SPECIFIC_GROUP_REPORT_V2         , &znet_cc_association_specific_group_report          , MESSAGE_TYPE_REPORT    },

    { /*0x86*/COMMAND_CLASS_VERSION                    , /*0x11*/VERSION_GET                                  , &znet_cc_version_request                            , MESSAGE_TYPE_SET_GET   },
    { /*0x86*/COMMAND_CLASS_VERSION                    , /*0x12*/VERSION_REPORT                               , &znet_cc_version_report                             , MESSAGE_TYPE_REPORT    },
    { /*0x86*/COMMAND_CLASS_VERSION                    , /*0x13*/VERSION_COMMAND_CLASS_GET                    , &znet_cc_command_class_version_request              , MESSAGE_TYPE_SET_GET   },
    { /*0x86*/COMMAND_CLASS_VERSION                    , /*0x14*/VERSION_COMMAND_CLASS_REPORT                 , &znet_cc_version_command_class_report               , MESSAGE_TYPE_REPORT    },
    { /*0x86*/COMMAND_CLASS_VERSION                    , /*0x16*/VERSION_CAPABILITIES_REPORT_V3               , &znet_cc_version_capabilities_report                , MESSAGE_TYPE_REPORT    },
    { /*0x86*/COMMAND_CLASS_VERSION                    , /*0x18*/VERSION_ZWAVE_SOFTWARE_REPORT_V3             , &znet_cc_version_software_report                    , MESSAGE_TYPE_REPORT    },

    { /*0x87*/COMMAND_CLASS_INDICATOR                  , /*0x01*/INDICATOR_SET_V2                             , &znet_cc_indicator_set_receive                      , MESSAGE_TYPE_SET_GET    },
    { /*0x87*/COMMAND_CLASS_INDICATOR                  , /*0x02*/INDICATOR_GET_V2                             , &znet_cc_indicator_get_receive                      , MESSAGE_TYPE_SET_GET    },
    { /*0x87*/COMMAND_CLASS_INDICATOR                  , /*0x03*/INDICATOR_REPORT                             , &znet_cc_indicator_report                           , MESSAGE_TYPE_REPORT     },
    { /*0x87*/COMMAND_CLASS_INDICATOR                  , /*0x04*/INDICATOR_SUPPORTED_GET_V2                   , &znet_cc_indicator_supported_get_receive            , MESSAGE_TYPE_SET_GET    },
    { /*0x87*/COMMAND_CLASS_INDICATOR                  , /*0x05*/INDICATOR_SUPPORTED_REPORT_V2                , &znet_cc_indicator_supported_report                 , MESSAGE_TYPE_REPORT     },

    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x01*/TIME_GET                                     , &znet_cc_local_time_requested                       , MESSAGE_TYPE_SET_GET   },
    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x02*/TIME_REPORT                                  , &znet_cc_local_time_received                        , MESSAGE_TYPE_REPORT    },
    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x03*/DATE_GET                                     , &znet_cc_local_date_requested                       , MESSAGE_TYPE_SET_GET   },
    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x04*/DATE_REPORT                                  , &znet_cc_local_date_received                        , MESSAGE_TYPE_REPORT    },
    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x05*/TIME_OFFSET_SET_V2                           , &znet_cc_time_zone_received                         , MESSAGE_TYPE_SET_GET    },
    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x06*/TIME_OFFSET_GET_V2                           , &znet_cc_time_zone_requested                        , MESSAGE_TYPE_SET_GET    },
    { /*0x8A*/COMMAND_CLASS_TIME                       , /*0x07*/TIME_OFFSET_REPORT_V2                        , &znet_cc_time_zone_report                           , MESSAGE_TYPE_REPORT    },

    { /*0x8B*/COMMAND_CLASS_TIME_PARAMETERS            , /*0x03*/TIME_PARAMETERS_REPORT                       , &znet_cc_time_parameters_report                     , MESSAGE_TYPE_REPORT    },

    { /*0x8E*/COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, /*0x01*/MULTI_CHANNEL_ASSOCIATION_SET_V3            , &znet_cc_multichannel_association_receive           , MESSAGE_TYPE_SET_GET   },
    { /*0x8E*/COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, /*0x02*/MULTI_CHANNEL_ASSOCIATION_GET_V3            , &znet_cc_multichannel_association_receive           , MESSAGE_TYPE_SET_GET   },
    { /*0x8E*/COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, /*0x03*/MULTI_CHANNEL_ASSOCIATION_REPORT_V3         , &znet_cc_multichannel_association_report            , MESSAGE_TYPE_REPORT    },
    { /*0x8E*/COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, /*0x04*/MULTI_CHANNEL_ASSOCIATION_REMOVE_V3         , &znet_cc_multichannel_association_receive           , MESSAGE_TYPE_SET_GET   },
    { /*0x8E*/COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, /*0x05*/MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V3  , &znet_cc_multichannel_association_receive           , MESSAGE_TYPE_SET_GET   },
    { /*0x8E*/COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, /*0x06*/MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V3, &znet_cc_multichannel_association_groupings_report  , MESSAGE_TYPE_REPORT    },

    { /*0x8F*/COMMAND_CLASS_MULTI_CMD                  , /*0x01*/MULTI_CMD_ENCAP                              , &znet_cc_multi_cmd_receive                          , MESSAGE_TYPE_TRANSPORT },

    { /*0x94*/COMMAND_CLASS_SIMPLE_AV_CONTROL          , /*0x01*/SIMPLE_AV_CONTROL_SET                        , &znet_cc_simple_av_set_report                       , MESSAGE_TYPE_REPORT    },

    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x02*/SECURITY_COMMANDS_SUPPORTED_GET              , &znet_cc_security_supported_get_recive              , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x03*/SECURITY_COMMANDS_SUPPORTED_REPORT           , &znet_cc_security_supported_report                  , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x04*/SECURITY_SCHEME_GET                          , &znet_cc_security_scheme_get_rec                    , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x05*/SECURITY_SCHEME_REPORT                       , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x06*/NETWORK_KEY_SET                              , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x07*/NETWORK_KEY_VERIFY                           , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x08*/SECURITY_SCHEME_INHERIT                      , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x40*/SECURITY_NONCE_GET                           , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x80*/SECURITY_NONCE_REPORT                        , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0x81*/SECURITY_MESSAGE_ENCAPSULATION               , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },
    { /*0x98*/COMMAND_CLASS_SECURITY                   , /*0xC1*/SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET     , &znet_cc_security_receive                           , MESSAGE_TYPE_TRANSPORT },

    { /*0x9C*/COMMAND_CLASS_SENSOR_ALARM               , /*0x02*/SENSOR_ALARM_REPORT                          , &znet_cc_sensor_alarm_report           	            , MESSAGE_TYPE_REPORT    },
    { /*0x9C*/COMMAND_CLASS_SENSOR_ALARM               , /*0x04*/SENSOR_ALARM_SUPPORTED_REPORT                , &znet_cc_sensor_alarm_supported_report              , MESSAGE_TYPE_REPORT    },

    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x01*/SECURITY_2_NONCE_GET                         , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x02*/SECURITY_2_NONCE_REPORT                      , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x03*/SECURITY_2_MESSAGE_ENCAPSULATION             , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x04*/KEX_GET                                      , &znet_cc_security_2_kex_get_received                , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x05*/KEX_REPORT                                   , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x06*/KEX_SET                                      , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x07*/KEX_FAIL                                     , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x08*/PUBLIC_KEY_REPORT                            , &znet_cc_security_2_receive                         , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x0A*/SECURITY_2_NETWORK_KEY_REPORT                , &znet_cc_security_2_network_key_report              , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x0D*/SECURITY_2_COMMANDS_SUPPORTED_GET            , &znet_cc_security_2_supported_get_receive           , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x0E*/SECURITY_2_COMMANDS_SUPPORTED_REPORT         , &znet_cc_security_2_supported_report_lib            , MESSAGE_TYPE_TRANSPORT },
    { /*0x9F*/COMMAND_CLASS_SECURITY_2                 , /*0x10*/SECURITY_2_CAPABILITIES_REPORT               , &znet_cc_security_2_capabilities_report             , MESSAGE_TYPE_TRANSPORT },

};
/* clang-format on */

#include "logger.h"
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
static int ICACHE_FLASH_ATTR CmpCBByCCIdAndCmd( const void* key_data, const int index ) {
    uint8_t* key = (uint8_t*)key_data;
    int result = (int)( *key ) - (int)cc_reports[index]._id;
    if( result != 0 )
        return result;
    return ( (int)( *( ++key ) ) - (int)cc_reports[index]._cmd );
}

//----------------------------------------------------------------------------
CC_Report ICACHE_FLASH_ATTR ZCommandClass_FindCCReportByCCData( const uint8_t* cc_report_data ) {
    int i = FindIndexByKey( cc_report_data, ARRAY_LEN( cc_reports ), &CmpCBByCCIdAndCmd );
    //      Log_Write("\r\nZCommandClass_FindCCReportByCCData i=%d \r\n",i);
    return ( ( i >= 0 ) ? cc_reports[i]._Report : NULL );
}

//----------------------------------------------------------------------------
uint8_t ICACHE_FLASH_ATTR ZCommandClass_FindTypeByCCData( const uint8_t* cc_report_data ) {
    int i = FindIndexByKey( cc_report_data, ARRAY_LEN( cc_reports ), &CmpCBByCCIdAndCmd );
    //      Log_Write("\r\nZCommandClass_FindCCReportByCCData i=%d \r\n",i);
    return ( ( i >= 0 ) ? cc_reports[i]._type : MESSAGE_TYPE_UNKNOWN );
}
