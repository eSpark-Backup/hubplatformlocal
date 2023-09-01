local params = ...
-- params = {
--     id = <cloud_device_id>
-- }

local cloud_devices = assert(require "cloud_devices", "Fail to load cloud devices functionality")
local core = assert(require "core", "Fail to load core functionality")

local plugin_gateway_id = core.get_gateway().id
local cloud_device = cloud_devices.get_device(params.id)

loadfile("HUB:cloud_devices/scripts/helpers/add_device")(plugin_gateway_id, cloud_device)
