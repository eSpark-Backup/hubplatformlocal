local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local cc_central_scene = {
    id = class_ids.central_scene,
    buttons = {
        {button_id = "1", button_attributes = {"press_1_time", "released", "held_down"}},
        {button_id = "2", button_attributes = {"press_1_time", "released", "held_down"}},
        {button_id = "3", button_attributes = {"press_1_time", "released", "held_down"}},
        {button_id = "4", button_attributes = {"press_1_time", "released", "held_down"}}
    }
}

local cc_notification = {
    id = class_ids.notification,
    notifications = {
        {
            type = "power_management",
            event_types = {
                "battery_is_charging",
                "battery_is_fully_charged",
                "charge_battery_soon",
                "charge_battery_now"
            }
        },
    }
}

local channel_0 = {
    id = 0,
    classes = { cc_central_scene, cc_notification }
}

local channel_1 = {
    id = 1,
    classes = { cc_central_scene }
}


local channel_2 = {
    id = 2,
    classes = { cc_central_scene }
}

local channel_3 = {
    id = 3,
    classes = { cc_central_scene }
}

local channel_4 = {
    id = 4,
    classes = { cc_central_scene }
}

local node = {
    id = "9000",
    manufacturer_id = 134,
    product_type = 258,
    product_id = 130,
    is_battery_powered = true,

    specific_type = "GENERIC_TYPE_WALL_CONTROLLER",
    generic_type = "GENERIC_TYPE_WALL_CONTROLLER",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0, channel_1, channel_2, channel_3, channel_4 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result
local expected_devices = {
    {
        name = "WallMote Quad",
        category = "scene_controller",
        items = {
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "1",
                value = {button_number = "1", button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "2",
                value = {button_number = "2", button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "3",
                value = {button_number = "3", button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "4",
                value = {button_number = "4", button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "battery_charging_state",
                tag = item_descriptor_tags.power_management,
                cc_id = class_ids.notification,
                channel_id = 0
            },
            {
                name = "battery",
                tag = item_descriptor_tags.power_management,
                cc_id = class_ids.notification,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",

        subdevices = {}
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
