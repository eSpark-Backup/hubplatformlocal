local flood_detector = {
    name = "Flood and Temperature Sensor",
    device_type = "flood_temperature",
    device_type_id = "flood_temperature",
    type = "sensor.multisensor",
    category = "security_sensor",
    subcategory = "leak",
    battery_powered = true,
    reachable = true
}

function flood_detector.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_water_leak_alarm_item(),
        item_factory.create_heat_alarm_item(),
        item_factory.create_freeze_alarm_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item(),
    }
end

function flood_detector.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        water_leak_alarm_id      = storage_utils.find_item_id(core_device_id, item_names.water_leak_alarm),
        heat_alarm_id            = storage_utils.find_item_id(core_device_id, item_names.heat_alarm),
        freeze_alarm_id          = storage_utils.find_item_id(core_device_id, item_names.freeze_alarm),
        security_threat_id       = storage_utils.find_item_id(core_device_id, item_names.security_threat),
        tampering_cover_alarm_id = storage_utils.find_item_id(core_device_id, item_names.tampering_cover_alarm),
        battery_id               = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return flood_detector
