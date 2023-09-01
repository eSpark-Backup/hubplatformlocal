local item_id, key = ...

local core = assert(require("core"), "Failed to load core functionality")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local item = assert(core.get_item(item_id), "Item not found")

if item.name == item_names.user_codes then
    local device_id = item.device_id
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local serial = storage_utils.find_serial(device_id)
    
    local keypad = require("HUB:2gig/scripts/model/devices/keypad")
    local keypad_item_ids = keypad.get_item_ids(serial)
    
    if keypad_item_ids.week_user_code_restriction_id then
        core.remove_item_dictionary_value(
            keypad_item_ids.week_user_code_restriction_id, 
            key
        )
    end
    
    if keypad_item_ids.day_user_code_restriction_id then
        core.remove_item_dictionary_value(
            keypad_item_ids.day_user_code_restriction_id, 
            key
        )
    end
    
    core.remove_item_dictionary_value(item_id, key)
    
elseif item.name == item_names.weekly_user_code_intervals or item.name == item_names.daily_user_code_intervals then
    core.remove_item_dictionary_value(item_id, key)
    loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
end
