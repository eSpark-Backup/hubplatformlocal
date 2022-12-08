local zwave_node, class_id = ...

class_id = tostring(math.floor(class_id))

local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(zwave_node.manufacturer_id, zwave_node.product_type, zwave_node.product_id)

local handler_names = hubplugins.weak_require("HUB:zwave/scripts/resources/specific_device_handler_names")
local device_category_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_category_info")(zwave_node, handler_names.command_class)

local command_class_location = "HUB:zwave/scripts/model/command_classes/"
return  hubplugins.object(class_id, command_class_location .. dev_handler_id)
    or  hubplugins.object(class_id, command_class_location .. device_category_info)
    or  hubplugins.object(class_id, command_class_location .. "default")
