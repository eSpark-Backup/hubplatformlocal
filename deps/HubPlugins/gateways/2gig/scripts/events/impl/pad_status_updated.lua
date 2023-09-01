local params = ...

local core = require("core")
local timer = require("timer")
local keypad = require("HUB:2gig/scripts/model/devices/keypad")
local button_states = require("HUB:2gig/scripts/enum/button_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = keypad.get_item_ids(params.serial)

local function switch_house_mode(mode_name)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    storage_utils.set_house_mode_to_change(mode_name)
    
    local timeout_setting_id = storage_utils.find_setting_id(storage_utils.find_device_id(params.serial), "pin_input_timeout")
    assert(timeout_setting_id)
    local setting = assert(core.get_setting(timeout_setting_id))
    
    timer.set_timeout(
        (setting.value * 2) * 1000,
        "HUB:2gig/scripts/helpers/clear_house_mode_to_change",
        {}
    )
end

local function panic(button_item_id, button_number)
    core.update_item_value(button_item_id, { button_number = button_number, button_state = button_states.held_down })
    core.update_item_value(item_ids.security_threat_id, true)
    core.update_item_value(button_item_id, { button_number = button_number, button_state = button_states.idle })
    core.update_item_value(item_ids.security_threat_id, false)
end

if params.button_pressed == "stay" then
    switch_house_mode("Home")
elseif params.button_pressed == "away" then
    switch_house_mode("Away")
elseif params.button_pressed == "fire" then
    panic(item_ids.fire_button_id, 1)
elseif params.button_pressed == "police" then
    panic(item_ids.police_button_id, 2)
end

core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
