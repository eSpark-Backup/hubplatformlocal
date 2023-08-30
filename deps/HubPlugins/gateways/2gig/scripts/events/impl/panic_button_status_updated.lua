local params = ...

local core = require("core")
local panic_button = require("HUB:2gig/scripts/model/devices/panic_button")
local button_states = require("HUB:2gig/scripts/enum/button_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = panic_button.get_item_ids(params.serial)

-- Note: ignore button released notifications, as those are unreliable
if params.button_pressed then
    core.update_item_value(item_ids.button_id, { button_number = 1, button_state = button_states.held_down })
    core.update_item_value(item_ids.security_threat_id, true)
    core.update_item_value(item_ids.button_id, { button_number = 1, button_state = button_states.idle })
    core.update_item_value(item_ids.security_threat_id, false)
end

core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
