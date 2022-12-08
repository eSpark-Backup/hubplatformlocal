local params = ...

local core = require("core")
local flood_temperature = require("HUB:2gig/scripts/model/devices/flood_temperature")
local water_leak_alarm_states = require("HUB:2gig/scripts/enum/water_leak_alarm_states")
local heat_alarm_states = require("HUB:2gig/scripts/enum/heat_alarm_states")
local freeze_alarm_states = require("HUB:2gig/scripts/enum/freeze_alarm_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local item_ids = flood_temperature.get_item_ids(params.serial)

core.update_item_value(item_ids.water_leak_alarm_id, water_leak_alarm_states.from_bool(params.water_detected))
core.update_item_value(item_ids.heat_alarm_id, heat_alarm_states.from_bool(params.heat_alarm))
core.update_item_value(item_ids.freeze_alarm_id, freeze_alarm_states.from_bool(params.freeze_alarm))
core.update_item_value(item_ids.security_threat_id, params.water_detected or params.heat_alarm or params.freeze_alarm)
core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
