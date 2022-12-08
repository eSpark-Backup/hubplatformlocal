local self, node_id, channel_id, class_id, type, value, scale, precision = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if class_id == class_ids.meter then
    local item_names = require("HUB:zwave/scripts/resources/item_names")
    local meter_info = hubplugins.weak_require("HUB:zwave/scripts/resources/meter_info")[type]
    if not meter_info then
        error("[handler.update_float_value]: unknown meter type " .. type)
    end
    
    local meter_item_info = meter_info[scale] 
    if not meter_item_info then
        error("[handler.update_float_value]: unknown meter scale " .. scale .. " of type " .. type)
    end

    local item_name = meter_item_info.item_name
    local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
    if not item_id then
        error("[handler.update_float_value]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))
    end

    value = value ~= 0 and (value / 10^precision) or value

    local result_value
    if meter_item_info.ui_scale then
        local core = require "core"
        local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")
        local item = core.get_item(item_id)
        local converted_value = unit_converter:call("convert", value, meter_item_info.ui_scale, item.value.scale)
        result_value = converted_value and 
            { value = converted_value, scale = item.value.scale } or 
            { value = value, scale = meter_item_info.ui_scale }
    else
        result_value = value
    end

    self:call("update_value_in_core", item_name, item_id, result_value)
end
