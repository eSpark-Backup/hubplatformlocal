local self, inclusion_context, build_tree_context, device_node = ...

local command_class = build_tree_context.command_class
assert(next(command_class.modes) ~= nil, "[default.68.process_inclusion_impl] array of thermostat fan modes is empty")

local item_names = require("HUB:zwave/scripts/resources/item_names")
local fan_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_fan_mode")

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.thermostat_fan_mode)

for _, mode in ipairs(command_class.modes) do
    item.enum[#item.enum + 1] = fan_mode_enum.value_to_token[mode]
end

item.value = assert(item.enum[1], "[default.68.process_inclusion_impl] unknown enum value")

self:call("add_item", inclusion_context, build_tree_context, device_node, item)
