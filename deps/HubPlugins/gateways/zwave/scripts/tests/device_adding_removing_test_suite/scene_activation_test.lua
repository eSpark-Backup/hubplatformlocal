local tests = require("tests")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local zwave = require("zwave")

zwave.request_node_values = tests.create_mock()

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLER",
    generic_type = "GENERIC_TYPE_GENERIC_CONTROLER",

    security = { "none" },
    security_class_started = { "none" },

    channels = {
        {
            id = 0,
            classes = {
                {
                    id = class_ids.scene_activation,
                    scene_actions = {
                        { scene_id = 1, button_number = 1, button_state = button_states.press_1_time },
                        { scene_id = 2, button_number = 2, button_state = button_states.press_1_time },
                        { scene_id = 3, button_number = 2, button_state = button_states.held_down },
                    }
                }
            }
        }
    }
}

zwave.add_node(node)

events_handler({
    event = "node_added",
    node_id = 9000
})

local function make_button_state_item(button_number, states)
    return {
        name = "button_state",
        tag = tostring(button_number),
        value_type = "button_state",
        enum = states,
        value = {
            button_number = button_number,
            button_state = button_states.idle
        },
        channel_id = 0,
        cc_id = class_ids.scene_activation
    }
end

local expected_devices = {
    {
        name = "Scene Controller",
        category = "scene_controller",
        subcategory = "generic",
        items = {
            make_button_state_item(1, { button_states.press_1_time, button_states.idle }),
            make_button_state_item(2, { button_states.press_1_time, button_states.held_down, button_states.idle })
        },
        node_id = "9000",
        battery_powered = false,
        security = "no",

        subdevices = {}
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices)
