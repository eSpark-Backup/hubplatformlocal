local self, setting_id, setting_descriptor, node_id, key, value = ...

local core = assert(require("core"), "Failed to load core functionality")
local zwave = assert(require("zwave"), "Failed to load zwave functionality")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if setting_descriptor and setting_descriptor.class_id == class_ids.configuration then
    value.status = "pending"
    if value.hasSetter == false then
        value.value = nil
    end

    core.set_setting_dictionary_value(setting_id, key, value)

    zwave.add_configuration(node_id, value.number, value.size)
end
