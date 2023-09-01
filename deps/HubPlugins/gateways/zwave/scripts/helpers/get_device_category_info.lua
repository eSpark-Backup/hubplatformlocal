local node_info, handler_names = ...

local function get_category_and_subcategory()
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node_info.manufacturer_id, node_info.product_type, node_info.product_id)
    local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/model/devices/devices_info/generated/" .. dev_handler_id)
    if device_info then device_info = device_info() end

    if device_info and device_info.category then
        return device_info.category, device_info.subcategory or ""
    end

    local generic_device_classes_info = hubplugins.weak_require("HUB:zwave/scripts/resources/generic_device_classes_info")

    local generic_device_class = generic_device_classes_info[node_info.generic_type]

    if not generic_device_class or not generic_device_class.specific_classes[node_info.specific_type] then
        return "", ""
    end

    if not generic_device_class.mandatory_command_classes then
        return "", ""
    end

    local does_node_contain_command_class = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/does_node_contain_command_class")
    local zwave = require("zwave")
    local node = zwave.get_node(node_info.id)
    for _, class_id in ipairs(generic_device_class.mandatory_command_classes) do
        if not does_node_contain_command_class(node, class_id) then
            return "", ""
        end
    end

    return generic_device_class.category, generic_device_class.subcategory or ""
end


local category, subcategory = get_category_and_subcategory()
local manufacturer_to_handler = handler_names[category]
local handler_name = manufacturer_to_handler and
                     ( manufacturer_to_handler[""] or manufacturer_to_handler[node_info.manufacturer_id] )

return handler_name or "", category, subcategory
