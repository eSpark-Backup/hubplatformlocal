local node_id = ...

local monitoring_class_location = "HUB:zwave/scripts/model/devices/monitoring"

if not node_id then
    return hubplugins.object("default", monitoring_class_location), "default"
end

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local key = storage:call("get_monitoring_handler_key", node_id)
local handler_info = storage:call("get_table", key)

if handler_info and handler_info.name then
    local zwave = require("zwave")

    local node_info = zwave.get_node_info(node_id)
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node_info.manufacturer_id, node_info.product_type, node_info.product_id)
    local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

    return hubplugins.object(handler_info.name, monitoring_class_location, node_id, device_info), handler_info.name
end

local handler, handler_name = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/_get_device_handler_monitoring_impl")(node_id)

if handler and handler_name then
    print("monitoring handler '" .. handler_name .. "' found for node " .. node_id)
    storage:call("set_table", key, { name = handler_name })
end

return handler, handler_name
