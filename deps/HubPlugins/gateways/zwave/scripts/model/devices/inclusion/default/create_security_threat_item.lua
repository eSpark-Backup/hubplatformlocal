local self, base_item = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local security_item = {
    name = item_names.security_threat,
    value_type = "bool",
    has_getter = true,
    has_setter = false,
    value = false
}

local security_tag_helper = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/security_tag_helper")()
security_item.descriptor = {
    item_name = base_item.name,
    node_id = base_item.descriptor.node_id,
    channel_id = base_item.descriptor.channel_id,
    class_id = base_item.descriptor.class_id,
    tag = security_tag_helper.add_security_tag(base_item.descriptor.tag)
}

return security_item
