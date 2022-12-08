local self, node_id, setting_descriptor, key, old_value, new_value = ...

local zwave = assert(require("zwave"), "Failed to load zwave functionality")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if not setting_descriptor then return end

if setting_descriptor.class_id == class_ids.configuration then
    if old_value ~= nil and old_value.number ~= new_value.number then
        zwave.remove_configuration(node_id, old_value.number)
    end

    if new_value.hasSetter ~= false then
        zwave.set_configuration_value(node_id, new_value.number, new_value.size, new_value.value)
    end

    self:call("request_configuration_value", node_id, new_value.number)

elseif setting_descriptor.class_id == nil and setting_descriptor.tag == "associations" then
    self:call("_set_associations_values", node_id, setting_descriptor, key, old_value, new_value)
end
