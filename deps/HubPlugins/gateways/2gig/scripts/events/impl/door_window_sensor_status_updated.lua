local params = ...

local core = require("core")
local door_window_sensor = require("HUB:2gig/scripts/model/devices/door_window_sensor")
local get_door_window_state = loadfile("HUB:2gig/scripts/helpers/get_door_window_state")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = door_window_sensor.get_item_ids(params.serial)

local dw_state = get_door_window_state(params.serial, params.reed_closed, params.external_contact)

core.update_item_value(item_ids.door_window_state_id, dw_state)
core.update_item_value(item_ids.security_threat_id, dw_state == "dw_is_opened")
core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
