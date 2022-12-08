local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local add_device = loadfile("HUB:2gig/scripts/add_device")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

-- Invalid format
add_device({
    serial = 123
})

-- Unknown device_type
add_device({
    serial = 123,
    device_type = "hello"
})

-- Unknown model
add_device({
    model = "hello",
    serial = 123
})

test_utils.check_devices(nil)

-- Unknown model, but known device_type
add_device({
    model = "hello",
    serial = 123,
    device_type = "panic_button"
})

test_utils.check_devices({
    {
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic",
        device_type_id = "panic_button",
        battery_powered = true,
        items = {
            item_factory.create_button_state_item(1, "held_down"),
            item_factory.create_security_threat_item(),                
            item_factory.create_battery_item()
        }
    }
})
