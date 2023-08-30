local params = ...

local core = require("core")

local supported_models = require("HUB:2gig/scripts/model/devices/generated/supported_devices")[params.device_type]

if supported_models ~= nil then
    core.send_ui_broadcast({
        event = "new_device_discovered",
        serial = params.serial,
        device_type = params.device_type,
        possible_models = supported_models
    })
else
    core.send_ui_broadcast({
        event = "unsupported_device_discovered",
        serial = params.serial
    })
end
