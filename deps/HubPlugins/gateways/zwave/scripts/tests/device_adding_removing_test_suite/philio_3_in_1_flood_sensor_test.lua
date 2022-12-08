local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local zwave = require( "zwave" )
local tests = require("tests")
local core = require( "core" )
local storage = require( "storage" )
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local item_names = require("HUB:zwave/scripts/resources/item_names")

zwave.request_node_values = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local binary_sensor_command_class = {
    id = class_ids.sensor_binary,
    sensor_types = {
        6,
        8
    }
}

local multilevel_sensor_command_class = {
    id = class_ids.multilevel_sensor,
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
                "percent"
            },
            sensor_type = "humidity"
        }
    }
}

local notification_command_class = {
    id = class_ids.notification,
    notifications = {
        {
            event_types = {
                "state_idle",
                "water_leak_detected"
            },
            type = "water_alarm",
            unknown_event_types =  {}
        },
        {
            event_types = {
                "product_cover_removed",
            },
            type = "home_security",
            unknown_event_types = {}
        }
    },
    v1_supported = false,
    version = 4
}

local channel_0 = {
    id = 0,
    classes = {
        notification_command_class,
        binary_sensor_command_class,
        multilevel_sensor_command_class
    }
}

local node = {
    id = "9000",
    manufacturer_id = 316,
    product_type = 2,
    product_id = 31,
    is_battery_powered = true,

    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0 }
}

local expected_parent_items = {
    {
        name = item_names.water_leak_alarm,
        cc_id = class_ids.notification,
        tag = item_tags.water_alarm,
        channel_id = 0,
    },
    {
        name = item_names.water_leak_alarm,
        cc_id = class_ids.sensor_binary,
        tag = item_tags.water_alarm,
        channel_id = 0,
        is_merged = true,
    },
    {
        name = item_names.security_threat,
        tag = "water_alarm.security_tag",
        cc_id = class_ids.notification,
        channel_id = 0,
        main_item_name = item_names.water_leak_alarm
    },
    {
        name = item_names.tampering_cover_alarm,
        tag = item_tags.home_security,
        cc_id = class_ids.notification,
        channel_id = 0,
    },
    {
        name = item_names.tampering_cover_alarm,
        tag = item_tags.home_security,
        cc_id = class_ids.sensor_binary,
        channel_id = 0,
        is_merged = true,
    },
}

local expected_temp_items = {
    {
        name = item_names.temp,
        cc_id = class_ids.multilevel_sensor,
        channel_id = 0,
    }
}

local expected_humidity_items = {
    {
        name = item_names.humidity,
        cc_id = class_ids.multilevel_sensor,
        channel_id = 0,
    }
}

local expected_devices = {
    {
        name = "Flood Sensor",
        category = "security_sensor",
        subcategory = "leak",
        items = expected_parent_items,
        node_id = "9000",
        battery_powered = true,
        subdevices = {
            {
                name = "Temperature Sensor",
                category = "temperature",
                subcategory = "",
                items = expected_temp_items
            },
            {
                name = "Humidity Sensor",
                category = "humidity",
                subcategory = "",
                items = expected_humidity_items
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

fixture:add_node(node)

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)

