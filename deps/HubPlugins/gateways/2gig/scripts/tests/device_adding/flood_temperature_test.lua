local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

test_utils.test_device_adding("2GIG", "FT1-345", "flood_temperature", {
    name = "Flood and Temperature Sensor",
    category = "security_sensor",
    subcategory = "leak",
    items = {
        item_factory.create_water_leak_alarm_item(),
        item_factory.create_heat_alarm_item(),
        item_factory.create_freeze_alarm_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item()
    }
})
