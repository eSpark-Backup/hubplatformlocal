local tests = require("tests")
local core = require("core")
local zwave = require("zwave")

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

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
                    parameters = {
                        {
                            default_value = 10,
                            format = "unsigned_integer",
                            id = 1,
                            info = "Number of minutes of _no_ motion before sending an OFF.",
                            max_value = 127,
                            min_value = 0,
                            name = "OnTime",
                            read_only = false,
                            size = 1
                        },
                        {
                            default_value = 255,
                            format = "unsigned_integer",
                            id = 2,
                            info = "Dim level (1-99%) sent to associated nodes when motion is detected.",
                            max_value = 255,
                            min_value = 0,
                            name = "OnLevel",
                            read_only = false,
                            size = 1
                        },
                        {
                            default_value = 60,
                            format = "unsigned_integer",
                            id = 3,
                            info = "Luminance report sent every LiteMin minutes.",
                            max_value = 255,
                            min_value = 1,
                            name = "LiteMin",
                            read_only = false,
                            size = 1
                        },
                        {
                            default_value = 60,
                            format = "unsigned_integer",
                            id = 4,
                            info = "Temperature report sent every TempMin minutes.",
                            max_value = 255,
                            min_value = 1,
                            name = "TempMin",
                            read_only = false,
                            size = 1
                        },
                        {
                            default_value = -40,
                            format = "signed_integer",
                            id = 5,
                            info = "Adjust the temperature by 0.1F. Range is +12.7F to -12.8F. Value of 1 raises by +0.1F. Value of -1 lowers by -0.1F. Value of 123 raises +12.3F.",
                            max_value = 127,
                            min_value = -128,
                            name = "TempAdj",
                            read_only = false,
                            size = 1
                        }
                    },
                    version = 3
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "motion_detected"
                            },
                            type = "home_security",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
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
                        },
                        {
                            scales = {
                                "percent",
                                "lux"
                            },
                            sensor_type = "illuminance"
                        }
                    },
                    version = 6
                },
                {
                    component_ids = {
                        "red",
                        "green",
                        "blue"
                    },
                    id = 51,
                    version = 1
                },
                {
                    id = 38,
                    version = 2
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    firmware_versions = {
                        "2.3"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "6.1"
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            }
                        }
                    },
                    association_groups_count = 2,
                    id = 133,
                    version = 2
                },
                {
                    firmware_ids = {
                        19785
                    },
                    id = 122,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    icon_type = "ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MOTION_ALARM",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 12,
    product_id = 1,
    product_type = 4,
    security = {
        "authenticated"
    },
    security_class_started = {
        "authenticated"
    },
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR"
}

fixture:add_node(node)

-- Check result
local expected_multi_sensor_items = {
    {
        name = item_names.rgb_color,
        cc_id = class_ids.switch_color,
        channel_id = 0,
    },
    {
        name = item_names.switch,
        cc_id = class_ids.dimmer,
        channel_id = 0,
    },
    {
        name = item_names.dimmer,
        cc_id = class_ids.dimmer,
        channel_id = 0,
    },
    {
        name = item_names.dimmer_up,
        cc_id = class_ids.dimmer,
        channel_id = 0,
    },
    {
        name = item_names.dimmer_down,
        cc_id = class_ids.dimmer,
        channel_id = 0,
    },
    {
        name = item_names.dimmer_stop,
        cc_id = class_ids.dimmer,
        channel_id = 0,
    },
}

local expected_motion_items = {
    {
        name = "motion",
        cc_id = class_ids.notification,
        tag = "home_security",
        channel_id = 0,
    },
    {
        name = "security_threat",
        tag = "home_security.security_tag",
        cc_id = class_ids.notification,
        channel_id = 0,
        main_item_name = "motion"
    }
}

local expected_devices = {
    {
        name = "Multi-Sensor",
        category = "dimmable_light",
        subcategory = "dimmable_colored",
        node_id = "9000",
        battery_powered = false,
        items = expected_multi_sensor_items,

        subdevices = {
            {
                name = "Motion Sensor",
                category = "security_sensor",
                subcategory = "motion",
                items = expected_motion_items
            },
            {
                name = "Light Level Sensor",
                category = "light_sensor",
                subcategory = "",
                items = {
                    {
                        name = item_names.lux,
                        cc_id = class_ids.multilevel_sensor,
                        channel_id = 0
                    }
                }
            },
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
                }
            }
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
