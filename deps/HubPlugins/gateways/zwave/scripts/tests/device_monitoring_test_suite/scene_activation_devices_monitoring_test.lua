local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

zwave.request_node_values = tests.create_mock()

core.update_item_value = tests.create_mock()

local function update_activated_scene_value(scene_id)
    events_handler({
        event = "scene_activated",
        node_id = 9000,
        class_id = class_ids.scene_activation,
        channel_id = 0,
        scene_id = scene_id
    })
end

local function check_button_state(button_number, state)
    local itemId = storage:call(
        "find_item_id",
        "button_state",             -- item_name
        9000,                       -- node_id
        class_ids.scene_activation, -- class_id
        0,                          -- channel_id
        button_number               -- tag
    )

    tests.assert_ne(itemId, nil, "Item should exist")

    tests.assert_called(core.update_item_value, "Check core.update_item_value call", {
        { itemId, { button_number = button_number, button_state = state } },
        { itemId, { button_number = button_number, button_state = button_states.idle } }
    })
end

local function test_impl(product_info)
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

    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node.manufacturer_id, node.product_type, node.product_id)
    local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

    tests.assert_ne(device_info.scene_actions, nil, "Check scene action described")

    for _, scene_info in ipairs(device_info.scene_actions) do
        update_activated_scene_value(scene_info.scene_id)
        check_button_state(scene_info.button_number, scene_info.button_state)
    end

    events_handler({
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    })
end

tests.run_with_params(test_impl, {
    {
        value = {
            manufacturer_id = 134,
            product_type = 1,
            product_id = 38
        }
    },
    {
        value = {
            manufacturer_id = 134,
            product_type = 1,
            product_id = 3,
        }
    },
    {
        value = {
            manufacturer_id = 134,
            product_type = 1,
            product_id = 38
        }
    },
    {
        value = {
            manufacturer_id = 134,
            product_type = 1,
            product_id = 22,
        }
    }
})
