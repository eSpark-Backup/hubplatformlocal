local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/panic_button_fixture")()
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")

zwave.request_node_values = tests.create_mock()

fixture:add_device_with_intrusion_alarm(
    {
        panic_triggers = {
            {button_number = 2, panic = { "held_down" }},
            {item_name = "intrusion_alarm", panic = { "intrusion" }},
            {item_name = "tampering_impact_alarm", panic = { "impact_detected" }}
        },
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic"
    }
)

local item_id_2 = storage:call(
    "find_item_id",
    "button_state",
    9000,
    class_ids.central_scene,
    0,
    "2"
)

local item_id_intrusion_alarm = storage:call(
    "find_item_id",
    "intrusion_alarm",
    9000,
    class_ids.notification,
    0,
    "home_security"
)

local item_id_tampering_impact_alarm = storage:call(
    "find_item_id",
    "tampering_impact_alarm",
    9000,
    class_ids.notification,
    0,
    "home_security"
)

local item_id_security = storage:call(
    "find_item_id",
    "button_state",
    9000,
    class_ids.central_scene,
    0,
    "2.security_tag"
)

assert(item_id_2, "Can not find button item 2 id")
assert(item_id_intrusion_alarm, "Can not find item intrusion_alarm id")
assert(item_id_tampering_impact_alarm, "Can not find item tampering_impact_alarm id")
assert(item_id_security, "Can not find item security id")

-- test button panic state
core.update_item_value = tests.create_mock()

events_handler({
    event = "button_state_updated",
    node_id = 9000,
    class_id = class_ids.central_scene,
    channel_id = 0,
    button_id = 2,
    value = "held_down",
})

tests.assert_called(
    core.update_item_value, 
    "Check core.update_item_value call", 
    {
        { item_id_2, { button_number = 2, button_state = "held_down" } },
        { item_id_security, true },
    })
    
-- test intrusion alarm notification
core.update_item_value = tests.create_mock()
events_handler( {
    event = "notification_recieved",
    node_id = 9000,
    class_id = class_ids.notification,
    channel_id = 0,
    v1_alarm_type = 0,
    v1_alarm_level = 0,
    event_type = "intrusion",
    value = 0xFF,
    notification_type = "home_security"
} )

tests.assert_called(
    core.update_item_value, 
    "Check core.update_item_value call", 
    {
        { item_id_intrusion_alarm, "intrusion" },
        { item_id_security, true },
    })
    
-- test impact alarm notification
core.update_item_value = tests.create_mock()
events_handler( {
    event = "notification_recieved",
    node_id = 9000,
    class_id = class_ids.notification,
    channel_id = 0,
    v1_alarm_type = 0,
    v1_alarm_level = 0,
    event_type = "impact_detected",
    value = 0xFF,
    notification_type = "home_security"
} )

tests.assert_called(
    core.update_item_value, 
    "Check core.update_item_value call", 
    {
        { item_id_tampering_impact_alarm, "impact_detected" },
        { item_id_security, true },
    })
