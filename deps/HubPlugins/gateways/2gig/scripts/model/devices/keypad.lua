local keypad = {
    name = "Keypad",
    device_type = "keypad",
    device_type_id = "keypad",
    type = "controller.remote",
    category = "keypad",
    subcategory = "",
    battery_powered = true,
    reachable = true,
    settings = {
        {
            tag = 'pin_length',
            label = 'Pin code length',
            description = 'Number of symbols in a pin code',
            type = 'number',
            default = 8,
            min = 4,
            max = 10
        },
        {
            tag = 'pin_input_timeout',
            label = 'Pin code input timeout',
            description = 'Maximum time in seconds between key presses',
            type = 'number',
            default = 5,
            min = 3,
            max = 5
        }
    }
}

function keypad.create_items()
    local item_factory = require("HUB:2gig/scripts/helpers/item_factory")

    return {
        item_factory.create_button_state_item(1, { "idle", "held_down" }),
        item_factory.create_button_state_item(2, { "idle", "held_down" }),
        item_factory.create_user_codes_item(),
        item_factory.create_user_code_action_item(),
        item_factory.create_week_user_code_restriction_item(),
        item_factory.create_day_user_code_restriction_item(),
        item_factory.create_security_threat_item(),
        item_factory.create_battery_item()
    }
end

function keypad.get_item_ids(serial)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local item_names = require("HUB:2gig/scripts/enum/item_names")

    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")

    return {
        fire_button_id                = storage_utils.find_item_id(core_device_id, item_names.button_state, 1),
        police_button_id              = storage_utils.find_item_id(core_device_id, item_names.button_state, 2),
        user_codes_id                 = storage_utils.find_item_id(core_device_id, item_names.user_codes),
        user_code_action_id           = storage_utils.find_item_id(core_device_id, item_names.user_code_action),
        week_user_code_restriction_id = storage_utils.find_item_id(core_device_id, item_names.weekly_user_code_intervals),
        day_user_code_restriction_id  = storage_utils.find_item_id(core_device_id, item_names.daily_user_code_intervals),
        security_threat_id            = storage_utils.find_item_id(core_device_id, item_names.security_threat),
        battery_id                    = storage_utils.find_item_id(core_device_id, item_names.battery)
    }
end

return keypad
