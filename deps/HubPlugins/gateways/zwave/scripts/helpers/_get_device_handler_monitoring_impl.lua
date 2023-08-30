local node_id = ...

local monitoring_class_location = "HUB:zwave/scripts/model/devices/monitoring"

if hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_broken_device_by_node_id")(node_id) then
    return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_fake_class")(
        function(t, k)
            return function( _, ...)
                print("invoked fake method: monitoring." .. k)
            end
        end)
end

local zwave = require("zwave")


local node_info = zwave.get_node_info(node_id)
local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node_info.manufacturer_id, node_info.product_type, node_info.product_id)
local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

local result = hubplugins.object(dev_handler_id, monitoring_class_location, node_id, device_info)
if result ~= nil then
    return result, dev_handler_id
end

local handler_names = hubplugins.weak_require("HUB:zwave/scripts/resources/specific_device_handler_names")
local device_category_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_category_info")(node_info, handler_names.monitoring)

result = hubplugins.object(device_category_info, monitoring_class_location, node_id, device_info)
if result ~= nil then
    return result, device_category_info
end

result = hubplugins.object("default", monitoring_class_location, node_id, device_info)
if result ~= nil then
    return result, "default"
end
