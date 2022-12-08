local params = ...

local core = require("core")
local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
local get_device_info = loadfile("HUB:2gig/scripts/helpers/get_device_info")

local honeywell_sensors_mapping = {
    door_window_sensor = { loop1 = "external_contact", loop2 = "reed_closed" },
    smoke_detector = { loop1 = "smoke_alarm", loop2 = "heat_alarm", loop3 = "freeze_alarm" },
    co_detector = { loop1 = "co_alarm" },
    smoke_co_detector = { loop1 = "alarm" },
    pir_detector = { loop1 = "motion_detected" },
    glass_break_detector = { loop1 = "glass_break_detected" },
    flood_temperature = { loop1 = "water_detected", loop2 = "heat_alarm", loop3 = "freeze_alarm" },
    tilt_detector = { loop1 = "tilt_detected" }
}

local device_id = storage_utils.find_device_id(params.serial)
local device = core.get_device(device_id)
local device_info = assert(get_device_info(device.device_type_id), "Unknown device")
local loop_replacement_map = assert(honeywell_sensors_mapping[device_info.device_type], "Unknown Honeywell device type")

params.event = device_info.device_type .. "_status_updated"
for loop_name, parameter_name in pairs(loop_replacement_map) do
    params[parameter_name] = params[loop_name]
end

loadfile("HUB:2gig/scripts/events/events_handling")(params)
