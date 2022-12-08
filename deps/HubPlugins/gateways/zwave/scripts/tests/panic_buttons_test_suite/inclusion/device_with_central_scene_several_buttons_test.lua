local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/panic_button_fixture")()
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require( "tests" )
local zwave = require( "zwave" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

fixture:add_device_with_central_scene(
    {
        panic_triggers = {
            {button_number = 1, panic = { "held_down" }},
            {button_number = 2, panic = { "held_down" }}
        },
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic"
    }
)

local function is_security_threat_merged(main_item_name, tag)
    local item_id = storage:call("find_item_id", 
                                main_item_name, 
                                9000, 
                                class_ids.central_scene, 
                                0, 
                                tag)
                                
    local main_descriptor = storage:call("find_item_descriptor", item_id)

    return main_descriptor.item_name ~= main_item_name or main_descriptor.tag ~= tag
end

local expected_devices = {
    {
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic",
        items = {
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "1",
                value = {button_number = 1, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "2",
                value = {button_number = 2, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "button_state",
                value_type = "button_state",
                enum = {"press_1_time", "released", "held_down", "idle"},
                tag = "3",
                value = {button_number = 3, button_state = "idle"},
                cc_id = class_ids.central_scene,
                channel_id = 0
            },
            {
                name = "security_threat",
                tag = "1.security_tag",
                cc_id = class_ids.central_scene,
                channel_id = 0,
                main_item_name = "button_state",
                is_merged = is_security_threat_merged("button_state", "1.security_tag")
            },
            {
                name = "security_threat",
                tag = "2.security_tag",
                cc_id = class_ids.central_scene,
                channel_id = 0,
                main_item_name = "button_state",
                is_merged = is_security_threat_merged("button_state", "2.security_tag")
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",

        subdevices = {}
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

fixture:remove_device_with_central_scene()

loadfile(test_suite_dir .. "check_zwave_node_removing_result")(check_context)
