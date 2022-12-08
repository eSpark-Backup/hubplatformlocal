local params = ...

local core = require("core")
local smoke_detector = require("HUB:2gig/scripts/model/devices/smoke_detector")
local smoke_alarm_states = require("HUB:2gig/scripts/enum/smoke_alarm_states")
local heat_alarm_states = require("HUB:2gig/scripts/enum/heat_alarm_states")
local freeze_alarm_states = require("HUB:2gig/scripts/enum/freeze_alarm_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = smoke_detector.get_item_ids(params.serial)

core.update_item_value(item_ids.smoke_alarm_id, smoke_alarm_states.from_bool(params.smoke_alarm))
core.update_item_value(item_ids.heat_alarm_id, heat_alarm_states.from_bool(params.heat_alarm))
core.update_item_value(item_ids.freeze_alarm_id, freeze_alarm_states.from_bool(params.freeze_alarm))
core.update_item_value(item_ids.security_threat_id, params.smoke_alarm or params.heat_alarm or params.freeze_alarm)
core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
