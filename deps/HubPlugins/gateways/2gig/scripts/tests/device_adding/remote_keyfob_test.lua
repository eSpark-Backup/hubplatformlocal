local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

test_utils.test_device_adding("2GIG", "KEY2-345", "remote_keyfob", {
    name = "Keyfob",
    category = "remote_control",
    subcategory = "",
    items = {
        item_factory.create_button_state_item(1, "held_down"),
        item_factory.create_button_state_item(2, "held_down"),
        item_factory.create_button_state_item(3, "held_down"),
        item_factory.create_button_state_item(4, "press_1_time"),
        item_factory.create_battery_item()
    }
})
