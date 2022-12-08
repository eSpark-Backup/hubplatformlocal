local core = require("core")
local storage = require("storage")
local timer = require("timer")
local tests = require( "tests" )
local keypad = require("HUB:2gig/scripts/model/devices/keypad")
local button_states = require("HUB:2gig/scripts/enum/button_states")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

add_device({
    model = "2GIG-PAD1-345",
    serial = 123
})

local item_ids = keypad.get_item_ids(123)

local function check_event(event_params, expected_call_params, house_mode_name)
    timer.set_timeout = tests.create_mock()

    send_event({
        event = "pad_status_updated",
        serial = 123,
        button_pressed = event_params.button_pressed,
        low_battery = event_params.low_battery
    })

    if house_mode_name ~= nil then
        tests.assert_eq(
            storage.get_string("2gig_house_mode_to_change"), 
            house_mode_name, 
            "Check new house mode saved"
        )
        tests.assert_called(timer.set_timeout, "Check house modes timer created", {
            { 10000, "HUB:2gig/scripts/helpers/clear_house_mode_to_change", {} }
        })
    end

    tests.assert_called(core.update_item_value, "Check core.update_item_value call", expected_call_params)
end

check_event(
    {
        button_pressed = "away",
        low_battery = true
    },
    {
        { item_ids.battery_id, 15 }
    },
    "Away"
)

check_event(
    {
        button_pressed = "away",
        low_battery = false
    },
    {
        { item_ids.battery_id, 100 }
    },
    "Away"
)

check_event(
    {
        button_pressed = "stay",
        low_battery = false
    },
    {
        { item_ids.battery_id, 100 }
    },
    "Home"
)

check_event(
    {
        button_pressed = "fire",
        low_battery = false
    },
    {
        { item_ids.fire_button_id, { button_number = 1, button_state = button_states.held_down } },
        { item_ids.security_threat_id, true },
        { item_ids.fire_button_id, { button_number = 1, button_state = button_states.idle } },
        { item_ids.security_threat_id, false },
        { item_ids.battery_id, 100 }
    }
)

check_event(
    {
        button_pressed = "police",
        low_battery = false
    },
    {
        { item_ids.police_button_id, { button_number = 2, button_state = button_states.held_down } },
        { item_ids.security_threat_id, true },
        { item_ids.police_button_id, { button_number = 2, button_state = button_states.idle } },
        { item_ids.security_threat_id, false },
        { item_ids.battery_id, 100 }
    }
)
