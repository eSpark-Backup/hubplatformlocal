local remote_keyfob = {
    name = "Keyfob",
    device_type = "remote_keyfob",
    device_type_id = "remote_keyfob",
    type = "controller.remote",
    category = "remote_control",
    subcategory = "",
    battery_powered = true,
    reachable = true
}

function remote_keyfob.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_button_state_item(1, { "idle", "held_down" }),
        item_factory.create_button_state_item(2, { "idle", "held_down" }),
        item_factory.create_button_state_item(3, { "idle", "held_down" }),
        item_factory.create_button_state_item(4, { "idle", "press_1_time" }),
        item_factory.create_battery_item(),
    }
end

function remote_keyfob.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        button1_id = storage_utils.find_item_id(core_device_id, item_names.button_state, 1),
        button2_id = storage_utils.find_item_id(core_device_id, item_names.button_state, 2),
        button3_id = storage_utils.find_item_id(core_device_id, item_names.button_state, 3),
        button4_id = storage_utils.find_item_id(core_device_id, item_names.button_state, 4),
        battery_id = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return remote_keyfob
