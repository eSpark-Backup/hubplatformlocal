local params = ...

local core = require("core")
local pir_detector = require("HUB:2gig/scripts/model/devices/pir_detector")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = pir_detector.get_item_ids(params.serial)

core.update_item_value(item_ids.motion_id, params.motion_detected)
core.update_item_value(item_ids.security_threat_id, params.motion_detected)
core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
