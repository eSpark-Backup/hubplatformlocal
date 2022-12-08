local self, item_name, item_id, value = ...

local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

core.update_item_value(item_id, value)

local item = core.get_item(item_id)
if self:call("is_security_item", item) then
    local security_tag_helper = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/security_tag_helper")()

    local find_item_id = function(descriptor, changed_tag)
        return self:call("find_item_id",
            descriptor.item_name,
            descriptor.node_id,
            descriptor.channel_id,
            descriptor.class_id,
            changed_tag
        )
    end

    local descriptor = storage:call("find_item_descriptor", item_id)
    local security_item_id = find_item_id(descriptor, security_tag_helper.add_security_tag(descriptor.tag))

    if security_item_id then
        local merged_security_descriptors = storage:call("find_item_descriptors", security_item_id )
        local security_value = self:call("get_security_threat_value", item, value)

        for _, security_descriptor in ipairs(merged_security_descriptors) do
            if not security_value and security_descriptor.item_name ~= item_name then
                local orig_item_id = find_item_id(security_descriptor,
                                                  security_tag_helper.remove_security_tag(security_descriptor.tag))

                local orig_item = core.get_item(orig_item_id)

                security_value = self:call("get_security_threat_value", orig_item, orig_item.value)
            end
        end

        core.update_item_value(security_item_id, security_value or false)
    else
        print("[handler.update_value_in_core] security item not found")
    end
end
