local core = require("core")
local tests = require( "tests" )
local panic_button = require("HUB:2gig/scripts/model/devices/panic_button")
local button_states = require("HUB:2gig/scripts/enum/button_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-PANIC1-345",
    serial = 123
})

local item_ids = panic_button.get_item_ids(123)

local function check_event(event_params, expected_call_params)
    send_event({
        event = "panic_button_status_updated",
        serial = 123,
        button_pressed = event_params.button_pressed,
        low_battery = event_params.low_battery
    })

    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

check_event(
    {
        button_pressed = false,
        low_battery = true
    },
    {
        { item_ids.battery_id, 15 },
    }
)

check_event(
    {
        button_pressed = true,
        low_battery = false
    },
    {
        { item_ids.button_id, { button_number = 1, button_state = button_states.held_down } },
        { item_ids.security_threat_id, true },
        { item_ids.button_id, { button_number = 1, button_state = button_states.idle } },
        { item_ids.security_threat_id, false },
        { item_ids.battery_id, 100 },
    }
)
