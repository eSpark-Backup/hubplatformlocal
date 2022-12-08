local glass_break_detector = {
    name = "Glass Break Sensor",
    device_type = "glass_break_detector",
    device_type_id = "glass_break_detector",
    type = "sensor",
    category = "security_sensor",
    subcategory = "glass",
    battery_powered = true,
    reachable = true
}

function glass_break_detector.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_glass_breakage_alarm_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item(),
    }
end

function glass_break_detector.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        glass_breakage_alarm_id  = storage_utils.find_item_id(core_device_id, item_names.glass_breakage_alarm),
        security_threat_id       = storage_utils.find_item_id(core_device_id, item_names.security_threat),
        tampering_cover_alarm_id = storage_utils.find_item_id(core_device_id, item_names.tampering_cover_alarm),
        battery_id               = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return glass_break_detector
