local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")
local setting_factory = require("HUB:2gig/scripts/helpers/setting_factory")

test_utils.test_device_adding("2GIG", "PAD1-345", "keypad", {
    name = "Keypad",
    category = "keypad",
    subcategory = "",
    items = {
        item_factory.create_button_state_item(1, "held_down"),
        item_factory.create_button_state_item(2, "held_down"),
        item_factory.create_user_codes_item(),
        item_factory.create_user_code_action_item(),
        item_factory.create_week_user_code_restriction_item(),
        item_factory.create_day_user_code_restriction_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_battery_item()
    },
    settings = {
        setting_factory.create_number_setting({
            tag = "pin_length",
            label = "Pin code length",
            description = "Number of symbols in a pin code",
            default = 8,
            min = 4,
            max = 10
        }),
        setting_factory.create_number_setting({
            tag = "pin_input_timeout",
            label = "Pin code input timeout",
            description = "Maximum time in seconds between key presses",
            default = 5,
            min = 3,
            max = 5
        })
    }
})
