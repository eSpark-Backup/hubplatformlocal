local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local items = {
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.thermostat_setpoint),
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.thermostat_setpoint_heating),
}

self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)

if hubplugins.device_settings_enabled then
    local setting = self:call("settings/controlled_setpoint_type", inclusion_context.zwave_node.id)
    local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

    local enum_setpoint = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")

    for _, setpoint in ipairs(build_tree_context.command_class.setpoints) do
        local token = setpoint.type and enum_setpoint.value_to_token[setpoint.type]
        if token then
            setting.enum[token] = { 
                text = token:gsub("_", " "):gsub("^%l", string.upper),
                lang_tag = "thermostat_setpoint_type_" .. token
            }
        end
    end

    table.insert(device_node_data.settings, setting)
end

