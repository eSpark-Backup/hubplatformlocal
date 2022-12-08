local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class
if cc.is_supported == false then
    print("Basic CC is not supported")
    return
end

local item_names = require("HUB:zwave/scripts/resources/item_names")

local cc_data = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_basic_cc_data_by_device_info")( inclusion_context.zwave_node, build_tree_context.channel.id )
if cc_data then
    local item_info = self:call( "get_item_info", cc_data )
    if item_info then
        local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_info.name)
        if item_info.to_child then
            self:call("add_complicated_items", inclusion_context, build_tree_context, device_node, { item })
        else
            self:call("add_item", inclusion_context, build_tree_context, device_node, item)
        end
        return
    end
end

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.basic)
self:call("add_item", inclusion_context, build_tree_context, device_node, item)
