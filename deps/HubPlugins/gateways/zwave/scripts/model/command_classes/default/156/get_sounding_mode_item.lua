local self, inclusion_context, build_tree_context, device_node, item_tag = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local sounding_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")

local sounding_mode_item = self:call("create_item", 
                                    inclusion_context, 
                                    build_tree_context, 
                                    device_node, 
                                    item_names.sounding_mode, 
                                    item_tag)

table.insert(sounding_mode_item.enum, sounding_mode_enum.audible)
table.insert(sounding_mode_item.enum, sounding_mode_enum.silent)

sounding_mode_item.has_setter = true
sounding_mode_item.has_getter = false

return sounding_mode_item
