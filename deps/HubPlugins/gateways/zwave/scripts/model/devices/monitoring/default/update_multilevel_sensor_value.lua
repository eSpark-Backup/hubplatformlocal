local self, node_id, channel_id, class_id, sensor_type, value, scale = ...

local multilevel_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/multilevel_sensor_map")
local item_name = multilevel_sensor_map.sensor_type_to_item_name_method(sensor_type)
local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
assert(item_id, "[handler.update_multilevel_sensor_value]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))

local result_value
if scale == "float" then
    result_value = value
else
    result_value = {
        value = value,
        scale = scale
    }
end

self:call("update_value_in_core", item_name, item_id, result_value)
