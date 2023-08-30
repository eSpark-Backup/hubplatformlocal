local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

test_utils.test_device_adding("2GIG", "GB1-345", "glass_break_detector", {
    name = "Glass Break Sensor",
    category = "security_sensor",
    subcategory = "glass",
    items = {
        item_factory.create_glass_breakage_alarm_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item()
    }
})
