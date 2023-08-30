local node = ...

local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(node.manufacturer_id, node.product_type, node.product_id)

local device_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

local handler_names = hubplugins.weak_require("HUB:zwave/scripts/resources/specific_device_handler_names")
local device_category_handler_name = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_category_info")(node, handler_names.inclusion)

local inclusion_class_location = "HUB:zwave/scripts/model/devices/inclusion"
return  hubplugins.object(dev_handler_id, inclusion_class_location, device_info)
    or  hubplugins.object(device_category_handler_name, inclusion_class_location, device_info)
    or  hubplugins.object("default", inclusion_class_location, device_info)
