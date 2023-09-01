local params = ...

local core = require("core")
local get_device_info = loadfile("HUB:2gig/scripts/helpers/get_device_info")

local device_info = assert(get_device_info(params.model, params.device_type), "Unknown device")

loadfile("HUB:2gig/scripts/helpers/add_device")(device_info, params.tx_ids)

core.send_ui_broadcast({
    event = "add_device",
    status = "finished"
})
