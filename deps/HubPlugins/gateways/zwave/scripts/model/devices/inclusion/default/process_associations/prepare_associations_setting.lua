local self, zwave_node = ...

local function supports_target_channels()
    local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")

    local _, channel = find_if(zwave_node.channels, function(channel) return channel.id == 0 end)
    if channel then
        local _, cc = find_if(channel.classes, function(cc) return cc.id == class_ids.multi_channel_associations end)
        if cc and cc.association_groups and next(cc.association_groups) then
            return true
        end
    end

    return false
end

return {
    label = {
        text = "Zwave Associations"
    },
    description = {
        text = "Setting to control device associations"
    },
    value_type = "dictionary.array.association",
    status = "synced",
    target_channel_supported = supports_target_channels(),
    value = {},
    descriptor = {
        node_id = zwave_node.id,
        class_id = nil,
        tag = "associations"
    }
}
