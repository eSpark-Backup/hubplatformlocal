local self, inclusion_context, build_tree_context, device_node, item_name, --[[opt]]item_descriptor_tag = ...

local item_factory = require("HUB:zwave/scripts/helpers/item_factory")

local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

local item = item_factory.create(item_name, device_node_data.info.device_type_id)

item.descriptor = {
    item_name = item_name,
    node_id = inclusion_context.zwave_node.id,
    channel_id = build_tree_context.channel.id,
    class_id = build_tree_context.command_class.id,
    tag = item_descriptor_tag,
}

local managing_commands = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/item_managing_commands")(item, build_tree_context.command_class)

if managing_commands then
    item.info = item.info or {}
    item.info["zwave.references"] = {
        {
            channel = build_tree_context.channel.id,
            class = build_tree_context.command_class.id,
            managing_commands = managing_commands
        }
    }
end

-- Process "readonly" field from devices_info.json
local endpoint_info = device_node_data.info.endpoints_info and device_node_data.info.endpoints_info[build_tree_context.channel.id]
if endpoint_info and endpoint_info.readonly then
    item.has_setter = false
end

return item
