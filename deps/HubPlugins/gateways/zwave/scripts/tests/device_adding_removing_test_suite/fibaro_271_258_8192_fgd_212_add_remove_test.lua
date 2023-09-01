local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local zwave = require( "zwave" )
local tests = require("tests")
local core = require( "core" )

zwave.request_node_values = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local meter_command_class = { -- 0x32
    id = class_ids.meter,
    reset_supported = true,
    type = "electric",
    scales = {
        "kilo_watt_hour",
        "watt",
    }
}

local notification_command_class = { -- 0x71
    id = class_ids.notification,
    notifications = {
        {
            type = "heat_alarm",
            event_types = {
                "overheat_detected"
            }
        },
        {
            type = "power_management",
            event_types = {
                "surge_detected",
                "voltage_drop_drift",
                "over_current_detected",
                "over_load_detected",
                "load_error"
            }
        },
        {
            type = "system",
            event_types = {
                "system_hardware_failure"
            }
        }
    }
}

local channel_0 = {
    id = 0,
    classes = {
        { id = 0x5E },
        { -- 0x20
            id = class_ids.basic
        },
        { id = 0x86 },
        { id = 0x72 },
        { -- 0x26
            id = class_ids.dimmer
        },
        { id = 0x59 },
        { id = 0x85 },
        -- { id = class_ids.configuration },
        {  -- 0x31
            id = class_ids.multilevel_sensor,
            sensors = {
                {
                    scales = { "watt", },
                    sensor_type = "power"
                }
            }
        },
        meter_command_class,
        { id = 0x8E },
        { id = 0x60 },
        notification_command_class,
    }
}

local channel_1 = {
    id = 1,
    classes = {
        { id = 0x5E },
        { -- 0x20
            id = class_ids.basic
        },
        { id = 0x86 },
        { -- 0x26
            id = class_ids.dimmer
        },
        { id = 0x85 },
        { id = 0x59 },
        { id = 0x8E },
        meter_command_class,
        {  -- 0x31
            id = class_ids.multilevel_sensor,
            sensors = {
                {
                    scales = { "watt", },
                    sensor_type = "power"
                }
            }
        },
        notification_command_class,
    }
}

local channel_2 = {
    id = 2,
    classes = {
        { id = 0x5E },
        { -- 0x20
            id = class_ids.basic
        },
        { id = 0x86 },
        { -- 0x26
            id = class_ids.dimmer
        },
        { id = 0x85 },
        { id = 0x59 },
        { id = 0x8E },
    }
}

local node = {
    id = "9000",
    manufacturer_id = 271,
    product_type = 258,
    product_id = 8192,
    is_battery_powered = false,

    generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
    icon_type = "ICON_TYPE_GENERIC_LIGHT_DIMMER_SWITCH",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0 , channel_1, channel_2 }
}

local expected_dimmer_items = {
    {
        name = "dimmer_up",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "dimmer_stop",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "switch",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "dimmer",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "dimmer_down",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "dimmer",
        cc_id = class_ids.basic,
        channel_id = 0,
        is_merged = true,        
    },
    {
        name = "hw_state",
        cc_id = class_ids.notification,
        tag = "system",
        channel_id = 0
    },
    {
        name = "voltage_drop_drift_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 0
    },
    {
        name = "load_error_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 0
    },
    {
        name = "power_surge_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 0
    },
    {
        name = "over_current_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 0
    },
    {
        name = "over_load_state",
        tag = "power_management",
        cc_id = class_ids.notification,
        channel_id = 0
    },
    {
        name = "meter_reset",
        cc_id = class_ids.meter,
        channel_id = 0
    },
    {
        name = "electric_meter_kwh",
        cc_id = class_ids.meter,
        channel_id = 0
    },
    {
        name = "electric_meter_watt",
        cc_id = class_ids.multilevel_sensor,
        channel_id = 0,
    },
    {
        name = "electric_meter_watt",
        cc_id = class_ids.meter,
        channel_id = 0,
        is_merged = true,
    },

    -- 1st channel
    {
        name = "dimmer",
        cc_id = class_ids.basic,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "dimmer_up",
        cc_id = class_ids.dimmer,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "dimmer_stop",
        cc_id = class_ids.dimmer,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "switch",
        cc_id = class_ids.dimmer,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "dimmer",
        cc_id = class_ids.dimmer,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "dimmer_down",
        cc_id = class_ids.dimmer,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "meter_reset",
        cc_id = class_ids.meter,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "electric_meter_kwh",
        cc_id = class_ids.meter,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "electric_meter_watt",
        cc_id = class_ids.multilevel_sensor,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "electric_meter_watt",
        cc_id = class_ids.meter,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "power_surge_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "voltage_drop_drift_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "over_current_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "over_load_state",
        tag = "power_management",
        cc_id = class_ids.notification,
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "load_error_state",
        cc_id = class_ids.notification,
        tag = "power_management",
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "hw_state",
        cc_id = class_ids.notification,
        tag = "system",
        channel_id = 1,
        is_merged = true,
    },

    -- 2nd channel
    {
        name = "dimmer",
        cc_id = class_ids.basic,
        channel_id = 2,
        is_merged = true,
    },
    {
        name = "dimmer_up",
        cc_id = class_ids.dimmer,
        channel_id = 2,
        is_merged = true,
    },
    {
        name = "dimmer_stop",
        cc_id = class_ids.dimmer,
        channel_id = 2,
        is_merged = true,
    },
    {
        name = "switch",
        cc_id = class_ids.dimmer,
        channel_id = 2,
        is_merged = true,
    },
    {
        name = "dimmer",
        cc_id = class_ids.dimmer,
        channel_id = 2,
        is_merged = true,
    },
    {
        name = "dimmer_down",
        cc_id = class_ids.dimmer,
        channel_id = 2,
        is_merged = true,
    },
}

local expected_heat_items = {
    {
        name = "heat_alarm",
        cc_id = class_ids.notification,
        tag = "heat_alarm",
        channel_id = 0
    },
    {
        name = "heat_alarm",
        cc_id = class_ids.notification,
        tag = "heat_alarm",
        channel_id = 1,
        is_merged = true,
    },
    {
        name = "security_threat",
        cc_id = class_ids.notification,
        main_item_name = "heat_alarm",
        tag = "heat_alarm.security_tag",
        channel_id = 0
    }
}

local expected_devices = {
    {
        name = "Dimmer 2",
        category = "dimmable_light",
        subcategory = "dimmable_in_wall",
        items = expected_dimmer_items,
        node_id = "9000",
        battery_powered = false,
        subdevices = {
            {
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                items = expected_heat_items
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

fixture:add_node(node)

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, check_context)

fixture:remove_node(node)

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)

