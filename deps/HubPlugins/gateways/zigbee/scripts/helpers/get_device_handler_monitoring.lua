local node = ...

local dev_handler_id = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/create_device_type_id")(node.manufacturer_id, node.model_id)

local base_handler_path = "HUB:zigbee/scripts/devices/monitoring/"
local handler_path =
    hubplugins.weak_loadfile(base_handler_path .. dev_handler_id .. "/handler") or
    hubplugins.weak_loadfile(base_handler_path .. "/default/handler")

assert(handler_path, "Failed to find monitoring handler: manufacturer_id: " .. node.manufacturer_id .. ", model_id: " .. node.model_id)

return handler_path()
