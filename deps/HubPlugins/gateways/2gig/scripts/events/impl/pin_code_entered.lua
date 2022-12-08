local params = ...

if params.pin_state ~= "valid" then
    return
end

local core = require("core")
local keypad = require("HUB:2gig/scripts/model/devices/keypad")
local user_code_actions = require("HUB:2gig/scripts/enum/user_code_actions")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")

local item_ids = keypad.get_item_ids(params.serial)

local function find_user_id()
    local user_codes = core.get_item(item_ids.user_codes_id)

    for key, value in pairs(user_codes.value) do
        if value.code == params.pin and value.mode ~= "disabled" then
            return math.floor(key)
        end
    end
end

local user_id = find_user_id()
if user_id ~= nil then
    core.update_item_value(item_ids.user_code_action_id, {
        action = user_code_actions.user_code_entered,
        userId = user_id
    })
    
    -- Change house mode
    local new_house_mode_name = storage_utils.find_house_mode_to_change()
    if new_house_mode_name then
        local house_modes = require("house_modes")
        local mode = house_modes.find_mode_by_name(new_house_mode_name)
        house_modes.cancel_switch()
        house_modes.switch(mode.id)
    end
else
    core.update_item_value(item_ids.user_code_action_id, {
        action = user_code_actions.invalid_user_code_entered,
        userId = -1
    })
end

core.update_item_value(item_ids.user_code_action_id, {
    action = user_code_actions.unknown,
    userId = -1
})

storage_utils.clear_house_mode_to_change()
