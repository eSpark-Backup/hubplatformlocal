local item_id = ...

local core = assert(require("core"), "Failed to load core functionality")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local function clear_item_dictionary(item)
    for key, _ in pairs(item.value) do
        core.remove_item_dictionary_value(item.id, key)
    end
end

local item = assert(core.get_item(item_id), "Item not found")

if item.name == item_names.user_codes then
    local device_id = item.device_id
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local serial = storage_utils.find_serial(device_id)
    
    local keypad = require("HUB:2gig/scripts/model/devices/keypad")
    local keypad_item_ids = keypad.get_item_ids(serial)
    
    if keypad_item_ids.week_user_code_restriction_id then
        clear_item_dictionary(
            assert(
                core.get_item(keypad_item_ids.week_user_code_restriction_id), 
                "Item not found"
            )
        )
    end
    
    if keypad_item_ids.day_user_code_restriction_id then
        clear_item_dictionary(
            assert(
                core.get_item(keypad_item_ids.day_user_code_restriction_id), 
                "Item not found"
            )
        )
    end
    
    clear_item_dictionary(item)
    
elseif item.name == item_names.weekly_user_code_intervals or item.name == item_names.daily_user_code_intervals then
    clear_item_dictionary(item)
    loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
end
