local core = require("core")
local tests = require( "tests" )
local house_modes = require("house_modes")
local storage = require("storage")
local keypad = require("HUB:2gig/scripts/model/devices/keypad")
local user_code_actions = require("HUB:2gig/scripts/enum/user_code_actions")
local send_event = loadfile("HUB:2gig/scripts/events/events_handling")
local add_device = loadfile("HUB:2gig/scripts/add_device")

core.update_item_value = tests.create_mock()

local function on_pin_code_entered(pin, status)
    send_event({
        event = "pin_code_entered",
        pin = pin,
        pin_state = status,
        serial = 123
    })
end

add_device({
    model = "2GIG-PAD1-345",
    serial = 123
})

local item_ids = keypad.get_item_ids(123)

-- Partial
on_pin_code_entered("1234567", "partial")
tests.assert_called(core.update_item_value, "check calls to core.update_item_value", {})

-- Timeout
on_pin_code_entered("", "timeout")
tests.assert_called(core.update_item_value, "check calls to core.update_item_value", {})

-- No user codes added
on_pin_code_entered("12345678", "valid")
tests.assert_called(core.update_item_value, "check calls to core.update_item_value", {
    { item_ids.user_code_action_id, { action = user_code_actions.invalid_user_code_entered, userId = -1 } },
    { item_ids.user_code_action_id, { action = user_code_actions.unknown, userId = -1 } }
})

-- Invalid
core.update_item_dictionary_value(item_ids.user_codes_id, "1", { code = "1234", name = "foo" })
on_pin_code_entered("12345678", "valid")
tests.assert_called(core.update_item_value, "check calls to core.update_item_value", {
    { item_ids.user_code_action_id, { action = user_code_actions.invalid_user_code_entered, userId = -1 } },
    { item_ids.user_code_action_id, { action = user_code_actions.unknown, userId = -1 } }
})

-- Valid
core.update_item_dictionary_value(item_ids.user_codes_id, "2", { code = "12345678", name = "bar" })
on_pin_code_entered("12345678", "valid")
tests.assert_called(core.update_item_value, "check calls to core.update_item_value", {
    { item_ids.user_code_action_id, { action = user_code_actions.user_code_entered, userId = 2 } },
    { item_ids.user_code_action_id, { action = user_code_actions.unknown, userId = -1 } }
})

-- Disabled
core.update_item_dictionary_value(item_ids.user_codes_id, "1", { code = "22345678", name = "foo", mode = "disabled" })
on_pin_code_entered("22345678", "valid")
tests.assert_called(core.update_item_value, "check calls to core.update_item_value", {
    { item_ids.user_code_action_id, { action = user_code_actions.invalid_user_code_entered, userId = -1 } },
    { item_ids.user_code_action_id, { action = user_code_actions.unknown, userId = -1 } }
})

-- Change house mode
house_modes.switch = tests.create_mock()
house_modes.cancel_switch = tests.create_mock()
storage.set_string("2gig_house_mode_to_change", "Away")

core.update_item_dictionary_value(item_ids.user_codes_id, "2", { code = "12345678", name = "bar" })
on_pin_code_entered("12345678", "valid")
tests.assert_called(house_modes.cancel_switch, "check house mode change cancel", nil, 1)

tests.assert_called(house_modes.switch, "check house mode changed", {
    { "2" }
})

tests.assert_eq(
    storage.get_string("2gig_house_mode_to_change"), 
    nil, 
    "Check no house mode saved"
)

-- No change house mode on wrong pin code
house_modes.switch = tests.create_mock()
house_modes.cancel_switch = tests.create_mock()
storage.set_string("2gig_house_mode_to_change", "Away")

core.update_item_dictionary_value(item_ids.user_codes_id, "2", { code = "12345678", name = "bar" })
on_pin_code_entered("12346678", "valid")
tests.assert_called(house_modes.cancel_switch, "check house mode change cancel", {})

tests.assert_called(house_modes.switch, "check house mode changed", {})

tests.assert_eq(
    storage.get_string("2gig_house_mode_to_change"), 
    nil, 
    "Check no house mode saved"
)
