local core = require("core")
local tests = require( "tests" )
local remote_keyfob = require("HUB:2gig/scripts/model/devices/remote_keyfob")
local button_states = require("HUB:2gig/scripts/enum/button_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-KEY2-345",
    serial = 123
})

local item_ids = remote_keyfob.get_item_ids(123)

local function check_event(event_params, expected_call_params)
    send_event({
        event = "remote_keyfob_status_updated",
        serial = 123,
        button1_pressed = event_params.button1_pressed,
        button2_pressed = event_params.button2_pressed,
        button3_pressed = event_params.button3_pressed,
        button4_pressed = event_params.button4_pressed,
        low_battery = event_params.low_battery
    })

    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

check_event(
    {
        button1_pressed = false,
        button2_pressed = false,
        button3_pressed = false,
        button4_pressed = false,
        low_battery = true
    },
    {
        { item_ids.battery_id, 15 },
    }
)

check_event(
    {
        button1_pressed = true,
        button2_pressed = true,
        button3_pressed = true,
        button4_pressed = true,
        low_battery = false
    },
    {
        { item_ids.button1_id, { button_number = 1, button_state = button_states.held_down } },
        { item_ids.button1_id, { button_number = 1, button_state = button_states.idle } },
        { item_ids.button2_id, { button_number = 2, button_state = button_states.held_down } },
        { item_ids.button2_id, { button_number = 2, button_state = button_states.idle } },
        { item_ids.button3_id, { button_number = 3, button_state = button_states.held_down } },
        { item_ids.button3_id, { button_number = 3, button_state = button_states.idle } },
        { item_ids.button4_id, { button_number = 4, button_state = button_states.press_1_time } },
        { item_ids.button4_id, { button_number = 4, button_state = button_states.idle } },
        { item_ids.battery_id, 100 },
    }
)
