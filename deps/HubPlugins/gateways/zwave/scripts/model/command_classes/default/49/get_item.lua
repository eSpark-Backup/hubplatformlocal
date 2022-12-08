local self, inclusion_context, build_tree_context, device_node, sensor_type = ...

local multilevel_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/multilevel_sensor_map")

return self:call("create_item", inclusion_context, build_tree_context, device_node, multilevel_sensor_map.sensor_type_to_item_name_method(sensor_type))
