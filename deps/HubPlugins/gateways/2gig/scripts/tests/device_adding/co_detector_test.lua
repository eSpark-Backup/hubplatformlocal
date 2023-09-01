local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

test_utils.test_device_adding("2GIG", "CO3-345", "co_detector", {
    name = "CO Detector",
    category = "security_sensor",
    subcategory = "co",
    items = {
        item_factory.create_co_alarm_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item()
    }
})
