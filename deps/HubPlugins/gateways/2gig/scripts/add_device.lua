local params = ...

local core = assert(require("core"), "Failed to load core functionality")
local twogig = assert(require("2gig"), "Failed to load 2gig functionality")
local get_device_info = loadfile("HUB:2gig/scripts/helpers/get_device_info")

local device_info = get_device_info(params.model, params.device_type)
if device_info ~= nil then
    if device_info.device_type == "smoke_co_detector" then
        twogig.add_device({
            tx_ids = { params.serial, params.serial + 1 },
            model = params.model,
            device_type = params.device_type
        })
    else
        twogig.add_device({
            tx_ids = { params.serial },
            model = params.model,
            device_type = params.device_type
        })
    end
else
    core.send_ui_broadcast({
        event = "add_device",
        status = "failed",
        details = "Device not supported"
    })
end
