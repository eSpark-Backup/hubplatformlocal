local node = ...

local dev_handler_id = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/create_device_type_id")(node.manufacturer_id, node.model_id)
local device_info = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_info")(dev_handler_id)

local base_handler_path = "HUB:zigbee/scripts/devices/inclusion/"
local handler_path =
    hubplugins.weak_loadfile(base_handler_path .. "device_specific/" .. dev_handler_id) or
    hubplugins.weak_loadfile(base_handler_path .. "handler")

assert(handler_path, "Failed to find inclusion handler: manufacturer_id: " .. node.manufacturer_id .. ", model_id: " .. node.model_id)

return handler_path(device_info)
