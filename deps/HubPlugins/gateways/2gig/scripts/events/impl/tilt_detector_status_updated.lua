local params = ...

local core = require("core")
local tilt_detector = require("HUB:2gig/scripts/model/devices/tilt_detector")
local door_window_states = require("HUB:2gig/scripts/enum/door_window_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = tilt_detector.get_item_ids(params.serial)

core.update_item_value(item_ids.door_window_state_id, door_window_states.from_bool(not params.tilt_detected))
core.update_item_value(item_ids.security_threat_id, params.tilt_detected)
core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
