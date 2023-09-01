local door_window_sensor = {
    name = "Door & Window Sensor",
    device_type = "door_window_sensor",
    device_type_id = "door_window_sensor",
    type = "sensor",
    category = "security_sensor",
    subcategory = "door",
    battery_powered = true,
    reachable = true
}

function door_window_sensor.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_door_window_state_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_tampering_cover_alarm_item(),
        item_factory.create_battery_item(),
    }
end

function door_window_sensor.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        door_window_state_id     = storage_utils.find_item_id(core_device_id, item_names.door_window_state),
        security_threat_id       = storage_utils.find_item_id(core_device_id, item_names.security_threat),
        tampering_cover_alarm_id = storage_utils.find_item_id(core_device_id, item_names.tampering_cover_alarm),
        battery_id               = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return door_window_sensor
