local params = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local smoke_co_detector = require("HUB:2gig/scripts/model/devices/smoke_co_detector")
local smoke_alarm_states = require("HUB:2gig/scripts/enum/smoke_alarm_states")
local co_alarm_states = require("HUB:2gig/scripts/enum/co_alarm_states")
local tampering_cover_states = require("HUB:2gig/scripts/enum/tampering_cover_states")
local battery_binary_to_persentage = loadfile("HUB:2gig/scripts/helpers/battery_binary_to_persentage")

local device_id = storage_utils.find_device_id(params.serial)
local device = core.get_device(device_id)
local is_smoke_detector = device.parent_device_id == ""

local item_ids

if is_smoke_detector then
    item_ids = smoke_co_detector.get_item_ids(params.serial)

    core.update_item_value(item_ids.smoke_alarm_id, smoke_alarm_states.from_bool(params.alarm))
    core.update_item_value(item_ids.security_threat_id, params.alarm)
else
    item_ids = smoke_co_detector.get_item_ids(params.serial - 1)
    local co_item_ids = smoke_co_detector.get_item_ids(params.serial)

    core.update_item_value(co_item_ids.co_alarm_id, co_alarm_states.from_bool(params.alarm))
    core.update_item_value(co_item_ids.security_threat_id, params.alarm)
end

core.update_item_value(item_ids.tampering_cover_alarm_id, tampering_cover_states.from_bool(params.tamper_detected))
core.update_item_value(item_ids.battery_id, battery_binary_to_persentage(params.low_battery))
