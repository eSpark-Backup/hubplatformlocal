local pir_detector = {
    name = "Motion Detector",
    device_type = "pir_detector",
    device_type_id = "pir_detector",
    type = "sensor.motion",
    category = "security_sensor",
    subcategory = "motion",
    battery_powered = true,
    reachable = true
}

function pir_detector.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_motion_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item(),
    }
end

function pir_detector.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        motion_id                = storage_utils.find_item_id(core_device_id, item_names.motion),
        security_threat_id       = storage_utils.find_item_id(core_device_id, item_names.security_threat),
        tampering_cover_alarm_id = storage_utils.find_item_id(core_device_id, item_names.tampering_cover_alarm),
        battery_id               = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return pir_detector
