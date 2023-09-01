local zwave = require("zwave")
local tests = require( "tests" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

zwave.request_node_values = tests.create_mock()

local channel_0 ={
    classes = {
    {
        id = 32,
        is_supported = false,
        version = 2
    },
    {
        id = 112,
        parameters = {
        {
            default_value = 0,
            format = "unsigned_integer",
            id = 16,
            info = "",
            max_value = 1,
            min_value = 0,
            name = "TEMPERATURE SCALE MODE",
            read_only = false,
            size = 4
        },
        {
            default_value = 10,
            format = "unsigned_integer",
            id = 17,
            info = "",
            max_value = 100,
            min_value = 1,
            name = "TEMPERATURE SENSOR OFFSET",
            read_only = false,
            size = 1
        },
        {
            default_value = 0,
            format = "unsigned_integer",
            id = 5,
            info = "",
            max_value = 1,
            min_value = 0,
            name = "RESERVED",
            read_only = false,
            size = 4
        },
        {
            default_value = 0,
            format = "unsigned_integer",
            id = 6,
            info = "",
            max_value = 1,
            min_value = 0,
            name = "RESERVED",
            read_only = false,
            size = 4
        }
        },
        version = 4
    },
    {
        id = 94,
        version = 2
    },
    {
        association_groups_count = 0,
        id = 133,
        version = 2
    },
    {
        association_groups = {
            {
                commands = {
                    {
                        class_id = 129,
                        command_id = 6,
                        name = "CLOCK_REPORT"
                    },
                    {
                        class_id = 113,
                        command_id = 5,
                        name = "NOTIFICATION_REPORT"
                    },
                    {
                        class_id = 37,
                        command_id = 3,
                        name = "SWITCH_BINARY_REPORT"
                    },
                    {
                        class_id = 50,
                        command_id = 2,
                        name = "METER_REPORT"
                    },
                    {
                        class_id = 49,
                        command_id = 5,
                        name = "SENSOR_MULTILEVEL_REPORT"
                    },
                    {
                        class_id = 90,
                        command_id = 1,
                        name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                    },
                    {
                        class_id = 135,
                        command_id = 3,
                        name = "INDICATOR_REPORT"
                    }
                },
                group_id = 1,
                group_name = "Lifeline",
                profile = {
                    name = "General:Lifeline",
                    profile_lsb = 1,
                    profile_msb = 0
                }
            },
            {
                commands = {
                    {
                        class_id = 32,
                        command_id = 1,
                        name = "BASIC_SET"
                    }
                },
                group_id = 2,
                group_name = "Relay Basic set",
                profile = {
                    name = "Control:Key01",
                    profile_lsb = 1,
                    profile_msb = 32
                }
            },
            {
                commands = {
                    {
                        class_id = 32,
                        command_id = 1,
                        name = "BASIC_SET"
                    }
                },
                group_id = 3,
                group_name = "S1 Basic set",
                profile = {
                    name = "Control:Key02",
                    profile_lsb = 2,
                    profile_msb = 32
                }
            },
            {
                commands = {
                    {
                        class_id = 49,
                        command_id = 5,
                        name = "SENSOR_MULTILEVEL_REPORT"
                    }
                },
                group_id = 4,
                group_name = "Temperature Sensor",
                profile = {
                    name = "Sensor:Air Temperature",
                    profile_lsb = 1,
                    profile_msb = 49
                }
            }
        },
        association_groups_count = 4,
        id = 89,
        version = 3
    },
    {
        association_groups = {
            {
                group_id = 1,
                group_nodes = {
                    {
                        endpoint_id = 0,
                        node_id = 1
                    }
                },
                group_nodes_max_count = 5
            },
            {
                group_id = 2,
                group_nodes_max_count = 5
            },
            {
                group_id = 3,
                group_nodes_max_count = 5
            },
            {
                group_id = 4,
                group_nodes_max_count = 5
            }
        },
        association_groups_count = 4,
        id = 142,
        version = 3
    },
    {
        application_build_number = 43707,
        application_framework_api_build_number = 43707,
        application_framework_api_version = "10.11.1",
        application_version = "10.11.1",
        firmware_versions = {
            "1.4"
        },
        hardware_version = 84,
        host_interface_build_number = 0,
        id = 134,
        library_type = "enhanced_slave",
        protocol_build_number = 175,
        protocol_version = "7.11.1",
        sdk_version = "7.11.1",
        version = 3,
        zwave_protocol_version = "7.11"
    },
    {
        id = 114,
        version = 2
    },
    {
        id = 129,
        version = 1
    },
    {
        id = 135,
        version = 3
    },
    {
        id = 152,
        version = 1
    },
    {
        id = 159,
        version = 1
    },
    {
        id = 37,
        version = 2
    },
    {
        id = 49,
        sensors = {
            {
                scales = {
                    "celsius",
                    "fahrenheit"
                },
                sensor_type = "air_temperature"
            }
        },
        version = 11
    },
    {
        id = 50,
        reset_supported = true,
        scales = {
            "kilo_watt_hour",
            "watt",
            "volt"
        },
        type = "electric",
        version = 5
    },
    {
        id = 113,
        notifications = {
            {
                event_types = {
                    "overheat_detected",
                    "rapid_temperature_rise"
                },
                type = "heat_alarm",
                unknown_event_types = {}
            },
            {
                event_types = {
                    "ac_mains_disconnected",
                    "ac_mains_reconnected",
                    "surge_detected",
                    "voltage_drop_drift",
                    "over_current_detected",
                    "over_voltage_detected",
                    "over_load_detected",
                    "load_error"
                },
                type = "power_management",
                unknown_event_types = {}
            },
            {
                event_types = {
                    "system_hardware_failure"
                },
                type = "system",
                unknown_event_types = {}
            }
        },
        v1_supported = false,
        version = 8
    },
    {
        firmware_ids = {
            1034
        },
        id = 122,
        version = 5
    }
},
    id = 0
}


local node = {
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 836,
    product_id = 10,
    product_type = 4,
    security = {
        "authenticated"
    },
    security_class_started = {
        "authenticated"
    },
    specific_type = "SPECIFIC_TYPE_NOT_USED",
    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local expected_settings = {
    {
        value_type = "int",
        value = 0,
        cc_id = class_ids.configuration,
        tag = "16",
        value_default = 0,
        value_min = 0,
        value_max = 1,
        info = {
            ["zwave.parameter_number"] = 16,
            ["zwave.parameter_size"] = 4
        }
    },
    {
        value_type = "int",
        value = 10,
        cc_id = class_ids.configuration,
        tag = "17",
        value_default = 10,
        value_min = 1,
        value_max = 100,
        info = {
            ["zwave.parameter_number"] = 17,
            ["zwave.parameter_size"] = 1
        }
    },
    {
        value_type = "action",
        value = {
            text = "Rediscover",
            lang_tag = "rediscover_tag"
        },
        tag = "rediscover"
    },
    {
        value_type = "action",
        value = { text = "Identify" },
        cc_id = class_ids.indicator,
        tag = "identify"
    },
    {
        value_type = "dictionary.association_group",
        tag = "associations_groups"
    },
    {
        value_type = "dictionary.array.association",
        tag = "associations"
    },
}

local expected_devices = {
    {
        name = "Switch Interrior",
        category = "switch",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0,
            },
            {
                name = "electric_meter_kwh",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "electric_meter_volt",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "meter_reset",
                cc_id = class_ids.meter,
                channel_id = 0
            },
            {
                name = "over_current_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "load_error_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "power_surge_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "over_load_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "voltage_drop_drift_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "over_voltage_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "ac_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = "hw_state",
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "system"
            }
        },
        settings = expected_settings,
        node_id = "9000",
        battery_powered = true,
        subdevices = {
            {
                name = "Temperature Sensor",
                category = "temperature",
                items = {
                    {
                        name = "temp",
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                items = {
                    {
                        name = "heat_alarm",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        tag = "heat_alarm"
                    },
                    {
                        name = "temperature_changes",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        tag = "heat_alarm"

                    },
                    {
                        name = "security_threat",
                        cc_id = class_ids.notification,
                        channel_id = 0,
                        tag = "heat_alarm.security_tag",
                        main_item_name = "heat_alarm"
                    }
                }
            },
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )
