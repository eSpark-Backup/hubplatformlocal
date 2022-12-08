local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local zwave = require("zwave")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = {
        { id = 0x5E },
        { id = 0x72 },
        { id = 0x2B },
        { id = 0x2C },
        -- { id = class_ids.configuration },
        { id = 0x86 },
        { id = 0x60 },
        { id = 0x8E },
        { id = 0x85 },
        { id = 0x59 },
        { id = class_ids.dimmer },
        {
            id = class_ids.notification,
            notifications = {
                {
                    type = "home_security",
                    event_types = { "motion_detected" }
                }
            }
        },
    }
}

local channel_1 = {
    id = 1,
    classes = {
        { id = 0x5E },
        { id = 0x59 },
        { id = class_ids.dimmer },
    }
}

local channel_2 = {
    id = 2,
    classes = {
        { id = 0x5E },
        { id = 0x59 },
        {
            id = class_ids.notification,
            notifications = {
                {
                    type = "home_security",
                    event_types = { "motion_detected" }
                }
            }
        },
    }
}

local node = {
    id = "9000",
    manufacturer_id = 99,
    product_type = 18765,
    product_id = 12340,
    is_battery_powered = false,

    generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
    icon_type = "ICON_TYPE_GENERIC_LIGHT_DIMMER_SWITCH",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0, channel_1, channel_2 }
}

fixture:add_node(node)

local expected_dimmer_items = {
    {
        name = "dimmer",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "dimmer_up",
        cc_id = class_ids.dimmer,
        channel_id = 0
    },
    {
        name = "dimmer_down",
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
        name = "dimmer_down",
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
}

local expected_motion_items = {
    {
        name = "motion",
        cc_id = class_ids.notification,
        tag = "home_security",
        channel_id = 0,
    },
    {
        name = "motion",
        cc_id = class_ids.notification,
        tag = "home_security",
        channel_id = 2,
        is_merged = true,
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
        name = "In-wall Smart Motion Dimmer",
        category = "dimmable_light",
        subcategory = "dimmable_in_wall",
        items = expected_dimmer_items,
        node_id = "9000",
        battery_powered = false,

        subdevices = {
            {
                name = "Motion Sensor",
                category = "security_sensor",
                subcategory = "motion",
                items = expected_motion_items
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