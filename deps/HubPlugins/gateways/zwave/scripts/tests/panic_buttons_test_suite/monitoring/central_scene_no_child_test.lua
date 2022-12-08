local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/panic_button_fixture")()
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require( "core" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

fixture:add_device_with_central_scene(
    {
        panic_triggers = {{button_number = 2, panic = { "held_down" }}},
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic"
    }
)

local item_id_1 = storage:call(
    "find_item_id",
    "button_state",
    9000,
    class_ids.central_scene,
    0,
    "1"
)

local item_id_2 = storage:call(
    "find_item_id",
    "button_state",
    9000,
    class_ids.central_scene,
    0,
    "2"
)

assert(item_id_1, "Can not find button item 1 id")
assert(item_id_2, "Can not find button item 2 id")

local security_item_id = storage:call(
    "find_item_id",
    "button_state",
    9000,
    class_ids.central_scene,
    0,
    "2.security_tag"
)

assert(security_item_id, "Can not find security item id")

local function test_impl(params)
    core.update_item_value = tests.create_mock()
    
    events_handler({
        event = "button_state_updated",
        node_id = 9000,
        class_id = class_ids.central_scene,
        channel_id = 0,
        button_id = params.button_id,
        value = params.value,
    })
    
    tests.assert_called(core.update_item_value, "Check core.update_item_value call", params.items_updates)
end

tests.run_with_params(test_impl, {
    {
        value = {
            button_id = 1,
            value = "press_1_time",
            items_updates = { 
                { item_id_1, { button_number = 1, button_state = "press_1_time" } }, 
                { item_id_1, { button_number = 1, button_state = "idle" } } 
            }
        }
    },
    {
        value = {
            button_id = 2,
            value = "press_1_time",
            items_updates = { 
                { item_id_2, { button_number = 2, button_state = "press_1_time" } },
                { security_item_id, false },
                { item_id_2, { button_number = 2, button_state = "idle" } },
                { security_item_id, false },
            }
        }
    },
    {
        value = {
            button_id = 2,
            value = "held_down",
            items_updates = { 
                { item_id_2, { button_number = 2, button_state = "held_down" } },
                { security_item_id, true },
            }
        }
    }
})
