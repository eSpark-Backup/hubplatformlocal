local params = ...

local core = require("core")
local co_detector = require("HUB:2gig/scripts/model/devices/co_detector")
local co_alarm_states = require("HUB:2gig/scripts/enum/co_alarm_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = co_detector.get_item_ids(params.serial)

core.update_item_value(item_ids.co_alarm_id, co_alarm_states.from_bool(params.co_alarm))
core.update_item_value(item_ids.security_threat_id, params.co_alarm)
core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
