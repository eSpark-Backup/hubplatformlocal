local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

test_utils.test_device_tree_adding("2GIG", "FF-345", "smoke_co_detector", {
    name = "Audio Detector",
    category = "security_sensor",
    subcategory = "smoke",
    items = {
        item_factory.create_smoke_alarm_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item()
    },
    child_devices = {
        {
            name = "Audio Detector 1",
            category = "security_sensor",
            subcategory = "co",
            items = {
                item_factory.create_co_alarm_item(),
                item_factory.create_security_threat_item()
            }
        }
    }
})
