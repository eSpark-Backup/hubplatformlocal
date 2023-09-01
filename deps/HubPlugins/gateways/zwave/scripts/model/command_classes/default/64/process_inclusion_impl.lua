local self, inclusion_context, build_tree_context, device_node = ...

local command_class = build_tree_context.command_class
assert(next(command_class.modes) ~= nil, "[default.64.process_inclusion_impl] array of thermostat modes is empty")

local item_names = require("HUB:zwave/scripts/resources/item_names")
local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.thermostat_mode)
local manuf_spec_mode = mode_enum.token_to_value["manufacturer_specific"]
local is_manuf_spec_supported

for _, mode in pairs(command_class.modes) do
    if mode == manuf_spec_mode then
        is_manuf_spec_supported = true
    end
    item.enum[#item.enum + 1] = mode_enum.value_to_token[mode]
end

item.value = assert(item.enum[1], "[default.64.process_inclusion_impl] unknown enum value")

self:call("add_item", inclusion_context, build_tree_context, device_node, item)

if is_manuf_spec_supported and hubplugins.device_settings_enabled then
    local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)
    local setting_tag = "thermostat_mode_manufacturer_specific_data";
    local setting = self:call("settings/" .. setting_tag)
    setting.descriptor = {
        node_id = inclusion_context.zwave_node.id,
        channel_id = build_tree_context.channel.id,
        class_id = command_class.id,
        tag = setting_tag
    }
    table.insert(device_node_data.settings, setting)
end
