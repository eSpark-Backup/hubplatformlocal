local params = ...

local core = require("core")
local remote_keyfob = require("HUB:2gig/scripts/model/devices/remote_keyfob")
local button_states = require("HUB:2gig/scripts/enum/button_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = remote_keyfob.get_item_ids(params.serial)

local function is_button_activated(button_number)
    local key = "button" .. button_number .. "_pressed"

    return params[key]
end

local function get_button_item_id(button_number)
    local key = "button" .. button_number .. "_id"

    return item_ids[key]
end

local function update_button_state(button_number, active_value)
    local button_item_id = get_button_item_id(button_number)

    -- Note: ignore button released notifications, as those are unreliable
    if is_button_activated(button_number) then
        core.update_item_value(button_item_id, { button_number = button_number, button_state = active_value })
        core.update_item_value(button_item_id, { button_number = button_number, button_state = button_states.idle })
    end
end

update_button_state(1, button_states.held_down)
update_button_state(2, button_states.held_down)
update_button_state(3, button_states.held_down)
update_button_state(4, button_states.press_1_time)

core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
