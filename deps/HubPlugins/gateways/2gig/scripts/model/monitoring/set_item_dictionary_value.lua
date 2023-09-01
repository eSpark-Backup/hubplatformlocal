local item_id, key, new_value = ...

local core = assert(require("core"), "Failed to load core functionality")
local item_names = require("HUB:2gig/scripts/enum/item_names")

local item = assert(core.get_item(item_id), "Item not found")

if item.name == item_names.user_codes then
    for id, code in pairs(item.value) do
        if id ~= key and code.code == new_value.code then
            print("Can not add new user code due to duplication")
            return
        end
    end
    
    core.update_item_dictionary_value(item_id, key, new_value)
    
elseif item.name == item_names.weekly_user_code_intervals or item.name == item_names.daily_user_code_intervals then
    local device_id = item.device_id
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local serial = storage_utils.find_serial(device_id)
    
    local keypad = require("HUB:2gig/scripts/model/devices/keypad")
    local keypad_item_ids = keypad.get_item_ids(serial)
    if keypad_item_ids.user_codes_id then
        local user_code_item = assert(core.get_item(keypad_item_ids.user_codes_id), "Item not found")
        for user_code_key, _ in pairs(user_code_item.value) do
            if user_code_key == key then
                core.update_item_dictionary_value(item_id, key, new_value)
                loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
                return
            end
        end
    end
end
