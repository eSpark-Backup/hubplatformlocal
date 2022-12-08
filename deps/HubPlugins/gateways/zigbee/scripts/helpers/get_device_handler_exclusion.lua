local manufacturer_id, model_id, dev_handler_id = ...

if not dev_handler_id then
    assert(manufacturer_id)
    assert(model_id)
    dev_handler_id = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/create_device_type_id")(manufacturer_id, model_id)
end

local base_handler_path = "HUB:zigbee/scripts/devices/exclusion/"
local handler_path =
    hubplugins.weak_loadfile(base_handler_path .. dev_handler_id) or
    hubplugins.weak_loadfile(base_handler_path .. "handler")

assert(handler_path, "Failed to find exclusion handler, dev_handler_id: " .. dev_handler_id)

return handler_path()
