local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

test_utils.test_device_adding("2GIG", "PANIC1-345", "panic_button", {
    name = "Panic Button",
    category = "scene_controller",
    subcategory = "panic",
    items = {
        item_factory.create_button_state_item(1, "held_down"),
        item_factory.create_security_threat_item(),
        item_factory.create_battery_item()
    }
})
