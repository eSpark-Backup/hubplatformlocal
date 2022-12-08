local self, inclusion_context, build_tree_context, device_node, item_tag = ...

local sounding_mode_item = self:_super_call("default/156", "get_sounding_mode_item", inclusion_context, build_tree_context, device_node, item_tag)

local sounding_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")
sounding_mode_item.enum = {
    sounding_mode_enum.unknown,
    sounding_mode_enum.silent
}

return sounding_mode_item
