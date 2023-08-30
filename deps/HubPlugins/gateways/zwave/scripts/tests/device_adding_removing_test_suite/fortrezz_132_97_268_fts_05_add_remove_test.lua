local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")
local tests = require( "tests" )

local function test_impl(params)
    zwave.request_node_values = tests.create_mock()

    local node = {
        basic_type = 4,
        channels = {
            {
                classes = {
                    {
                        id = 32,
                        is_supported = false,
                        version = 1
                    },
                    {
                        id = 112,
                        parameters = {},
                        version = 1
                    },
                    {
                        id = 94,
                        version = 2
                    },
                    {
                        firmware_versions = {
                            "2.12"
                        },
                        hardware_version = 255,
                        id = 134,
                        library_type = "enhanced_slave",
                        version = 2,
                        zwave_protocol_version = "4.38"
                    },
                    {
                        id = 114,
                        version = 2
                    },
                    {
                        id = 128,
                        version = 1
                    },
                    {
                        id = 113,
                        notifications = {
                            {
                                event_types = {
                                    "overheat_detected"
                                },
                                type = "heat_alarm",
                                unknown_event_types = {
                                    6
                                }
                            },
                            {
                                event_types = {
                                    "water_leak_detected"
                                },
                                type = "water_alarm",
                                unknown_event_types = {}
                            },
                            {
                                event_types = {
                                    "power_has_been_applied",
                                    "replace_battery_soon",
                                    "replace_battery_now"
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
                        version = 4
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes = {
                                    1
                                },
                                group_nodes_max_count = 1
                            },
                            {
                                group_id = 2,
                                group_nodes_max_count = 2
                            },
                            {
                                group_id = 3,
                                group_nodes_max_count = 2
                            },
                            {
                                group_id = 4,
                                group_nodes_max_count = 2
                            },
                            {
                                group_id = 5,
                                group_nodes_max_count = 2
                            },
                            {
                                group_id = 6,
                                group_nodes_max_count = 2
                            },
                            {
                                group_id = 7,
                                group_nodes_max_count = 2
                            },
                            {
                                group_id = 8,
                                group_nodes_max_count = 2
                            }
                        },
                        association_groups_count = 8,
                        id = 133,
                        version = 2
                    },
                    {
                        association_groups_count = 0,
                        id = 89,
                        version = 1
                    },
                    {
                        id = 132,
                        max_wake_up_interval = 86400,
                        min_wake_up_interval = 60,
                        step_wake_up_interval = 60,
                        version = 2,
                        wake_up_interval = 5400
                    },
                    {
                        id = 49,
                        sensors = {
                            {
                                scales = {
                                    "celsius"
                                },
                                sensor_type = "air_temperature"
                            }
                        },
                        version = 9
                    },
                    {
                        id = 152,
                        version = 0
                    },
                    {
                        firmware_ids = {
                            524
                        },
                        id = 122,
                        version = 2
                    }
                },
                id = 0
            }
        },
        generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
        icon_type = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_WATER_ALARM",
        id = "9000",
        is_battery_powered = true,
        is_reachable = false,
        is_rediscover_required = false,
        is_smart_include = false,
        manufacturer_id = params.manufacturer_id,
        product_id = params.product_id,
        product_type = params.product_type,
        security = {
            "s0"
        },
        security_class_started = {
            "s0"
        },
        specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",
        wake_up_interval = 5400
    }

    fixture:add_node(node)

    local expected_devices = {
        {
            name = "Flood and Temp Sensor",
            category = "security_sensor",
            subcategory = "leak",
            items = {
                {
                    name = item_names.battery,
                    cc_id = class_ids.battery,
                    channel_id = 0
                },
                {
                    name = item_names.battery_maintenance_state,
                    tag = "power_management",
                    cc_id = class_ids.notification,
                    channel_id = 0
                },
                {
                    name = item_names.power_state,
                    tag = "power_management",
                    cc_id = class_ids.notification,
                    channel_id = 0
                },
                {
                    name = "hw_state",
                    cc_id = class_ids.notification,
                    tag = "system",
                    channel_id = 0
                },
                {
                    name = item_names.water_leak_alarm,
                    tag = item_descriptor_tags.water_alarm,
                    cc_id = class_ids.notification,
                    channel_id = 0,
                },
                {
                    name = item_names.security_threat,
                    tag = "water_alarm.security_tag",
                    cc_id = class_ids.notification,
                    channel_id = 0,
                    main_item_name = item_names.water_leak_alarm
                }
            },
            settings = {
                {
                    value_type = "action",
                    value = {
                        text = "Rediscover",
                        lang_tag = "rediscover_tag"
                    },
                    tag = "rediscover"
                },
                {
                    cc_id = class_ids.configuration,
                    value_type = "dictionary.zwave_configuration",
                    value = {}
                },
                {
                    value_type = "int",
                    value = 5400,
                    value_default = 5400,
                    cc_id = class_ids.wake_up,
                    tag = "wake_up_interval",
                    value_min = 60,
                    value_max = 86400,
                    value_step = 60,
                },
                {
                    has_setter = false,
                    value = {
                        ["0_1"] = {
                            channel = 0,
                            commands = {
                                "BATTERY_REPORT",
                                "DEVICE_RESET_LOCALLY_NOTIFICATION",
                                "NOTIFICATION_REPORT",
                                "SENSOR_MULTILEVEL_REPORT"
                            },
                            maxAssociationsNumber = 1,
                            name = "Lifeline",
                            profile = "General:Lifeline"
                        },
                        ["0_2"] = {
                            channel = 0,
                            commands = {
                                "BASIC_SET"
                            },
                            maxAssociationsNumber = 2,
                            name = "Water Leak",
                            profile = "Control:Key01"
                        },
                        ["0_3"] = {
                            channel = 0,
                            commands = {
                                "BASIC_SET"
                            },
                            maxAssociationsNumber = 2,
                            name = "Over Temperature",
                            profile = "Control:Key02"
                        },
                        ["0_4"] = {
                            channel = 0,
                            commands = {
                                "BASIC_SET"
                            },
                            maxAssociationsNumber = 2,
                            name = "Under Temperature",
                            profile = "Control:Key03"
                        },
                        ["0_5"] = {
                            channel = 0,
                            commands = {
                                "SENSOR_MULTILEVEL_REPORT"
                            },
                            maxAssociationsNumber = 2,
                            name = "Temp-Upon Change",
                            profile = "Sensor:Air Temperature"
                        },
                        ["0_6"] = {
                            channel = 0,
                            commands = {
                                "SENSOR_MULTILEVEL_REPORT"
                            },
                            maxAssociationsNumber = 2,
                            name = "Temp-Every 5 min",
                            profile = "Sensor:Air Temperature"
                        },
                        ["0_7"] = {
                            channel = 0,
                            commands = {
                                "SENSOR_MULTILEVEL_REPORT"
                            },
                            maxAssociationsNumber = 2,
                            name = "Temp-Every 20 min",
                            profile = "Sensor:Air Temperature"
                        },
                        ["0_8"] = {
                            channel = 0,
                            commands = {
                                "SENSOR_MULTILEVEL_REPORT"
                            },
                            maxAssociationsNumber = 2,
                            name = "Temp-Every 60 min",
                            profile = "Sensor:Air Temperature"
                        }
                    },
                    value_type = "dictionary.association_group",
                    tag = "associations_groups"
                },
                {
                    value = {},
                    value_type = "dictionary.array.association",
                    tag = "associations"
                }
            },
            node_id = "9000",
            battery_powered = true,
            security = "low",

            subdevices = {
                {
                    name = "Temperature Sensor",
                    category = "temperature",
                    subcategory = "",
                    items = {
                        {
                            name = item_names.temp,
                            cc_id = class_ids.multilevel_sensor,
                            channel_id = 0
                        }
                    },
                },
                {
                    name = "Heat Sensor",
                    category = "security_sensor",
                    subcategory = "heat",
                    items = {
                        {
                            name = item_names.heat_alarm,
                            tag = item_descriptor_tags.heat_alarm,
                            cc_id = class_ids.notification,
                            channel_id = 0,
                        },
                        {
                            name = item_names.security_threat,
                            tag = "heat_alarm.security_tag",
                            cc_id = class_ids.notification,
                            channel_id = 0,
                            main_item_name = item_names.heat_alarm
                        }
                    },
                },
            }
        }
    }

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {},
        tests = tests
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

    fixture:remove_node(node)

    loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                manufacturer_id = 132,
                product_id = 524,
                product_type = 115
            },
            description = "test with 132_115_524"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 268,
                product_type = 99
            },
            description = "test with 132_99_268"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 268,
                product_type = 97
            },
            description = "test with 132_97_268"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 414,
                product_type = 99
            },
            description = "test with 132_99_414"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 257,
                product_type = 115
            },
            description = "test with 132_115_257"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 523,
                product_type = 115
            },
            description = "test with 132_115_523"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 258,
                product_type = 115
            },
            description = "test with 132_115_258"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 259,
                product_type = 115
            },
            description = "test with 132_115_259"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 261,
                product_type = 115
            },
            description = "test with 132_115_261"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 513,
                product_type = 115
            },
            description = "test with 132_115_513"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 514,
                product_type = 115
            },
            description = "test with 132_115_514"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 3,
                product_type = 115
            },
            description = "test with 132_115_3"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 4,
                product_type = 115
            },
            description = "test with 132_115_4"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 5,
                product_type = 115
            },
            description = "test with 132_115_5"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 6,
                product_type = 115
            },
            description = "test with 132_115_6"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 7,
                product_type = 115
            },
            description = "test with 132_115_7"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 8,
                product_type = 115
            },
            description = "test with 132_115_8"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 9,
                product_type = 115
            },
            description = "test with 132_115_9"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 10,
                product_type = 115
            },
            description = "test with 132_115_10"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 11,
                product_type = 115
            },
            description = "test with 132_115_11"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 12,
                product_type = 115
            },
            description = "test with 132_115_12"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 13,
                product_type = 115
            },
            description = "test with 132_115_13"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 14,
                product_type = 115
            },
            description = "test with 132_115_14"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 8,
                product_type = 163
            },
            description = "test with 132_163_8"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 257,
                product_type = 163
            },
            description = "test with 132_163_257"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 258,
                product_type = 163
            },
            description = "test with 132_163_258"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 259,
                product_type = 163
            },
            description = "test with 132_163_259"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 261,
                product_type = 163
            },
            description = "test with 132_163_261"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 513,
                product_type = 163
            },
            description = "test with 132_163_513"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 514,
                product_type = 163
            },
            description = "test with 132_163_514"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 515,
                product_type = 163
            },
            description = "test with 132_163_515"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 516,
                product_type = 163
            },
            description = "test with 132_163_516"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 517,
                product_type = 163
            },
            description = "test with 132_163_517"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 518,
                product_type = 163
            },
            description = "test with 132_163_518"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 519,
                product_type = 163
            },
            description = "test with 132_163_519"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 520,
                product_type = 163
            },
            description = "test with 132_163_520"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 521,
                product_type = 163
            },
            description = "test with 132_163_521"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 523,
                product_type = 163
            },
            description = "test with 132_163_523"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 524,
                product_type = 163
            },
            description = "test with 132_163_524"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 525,
                product_type = 163
            },
            description = "test with 132_163_525"
        },
        {
            value = {
                manufacturer_id = 132,
                product_id = 526,
                product_type = 163
            },
            description = "test with 132_163_526"
        }
    }
)
