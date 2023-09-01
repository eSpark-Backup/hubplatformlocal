local panic_button = {
    name = "Panic Button",
    device_type = "panic_button",
    device_type_id = "panic_button",
    type = "controller.panic",
    category = "scene_controller",
    subcategory = "panic",
    battery_powered = true,
    reachable = true
}

function panic_button.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_button_state_item(1, { "idle", "held_down" }),
        item_factory.create_security_threat_item(),        
        item_factory.create_battery_item(),
    }
end

function panic_button.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        button_id          = storage_utils.find_item_id(core_device_id, item_names.button_state, 1),
        security_threat_id = storage_utils.find_item_id(core_device_id, item_names.security_threat),
        battery_id         = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return panic_button
