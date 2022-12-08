local tests = require("tests")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local zwave = require("zwave")

zwave.request_node_values = tests.create_mock()

local function test_impl(product_info, expected_items)
    local node = {
        id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = false,

        specific_type = "SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLER",
        generic_type = "GENERIC_TYPE_GENERIC_CONTROLER",

        security = { "none" },
        security_class_started = { "none" },

        channels = {
            {
                id = 0,
                classes = {}
            }
        }
    }

    zwave.add_node(node)

    events_handler({
        event = "node_added",
        node_id = 9000
    })

    local expected_devices = {
        {
            name = product_info.name,
            category = product_info.category,
            subcategory = product_info.subcategory,
            items = expected_items,
            node_id = "9000",
            battery_powered = false,
            security = "no",

            subdevices = {}
        }
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices)

    events_handler({
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    })
end

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

test_impl(
    {
        manufacturer_id = 134,
        product_type = 1,
        product_id = 38,
        name = "Panic Button",
        category = "remote_control"
    },
    {
        make_button_state_item(1, { button_states.press_1_time, button_states.held_down, button_states.idle })
    }
)

test_impl(
    {
        manufacturer_id = 134,
        product_type = 1,
        product_id = 3,
        name = "Minimote",
        category = "remote_control"
    },
    {
        make_button_state_item(1, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(2, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(3, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(4, { button_states.press_1_time, button_states.held_down, button_states.idle })
    }
)

test_impl(
    {
        manufacturer_id = 134,
        product_type = 257,
        product_id = 88,
        name = "Keyfob Gen5",
        category = "remote_control"
    },
    {
        make_button_state_item(1, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(2, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(3, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(4, { button_states.press_1_time, button_states.held_down, button_states.idle })
    }
)

test_impl(
    {
        manufacturer_id = 134,
        product_type = 1,
        product_id = 22,
        name = "Keyfob Gen5",
        category = "remote_control"
    },
    {
        make_button_state_item(1, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(2, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(3, { button_states.press_1_time, button_states.held_down, button_states.idle }),
        make_button_state_item(4, { button_states.press_1_time, button_states.held_down, button_states.idle })
    }
)
