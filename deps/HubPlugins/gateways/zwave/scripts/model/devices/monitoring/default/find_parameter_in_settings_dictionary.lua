local self, node_id, parameter_number = ...

local core = assert(require("core"), "Failed to load core functionality")
local class_id = require("HUB:zwave/scripts/resources/class_ids").configuration

local setting = self:call("find_setting", node_id, class_id)

if not setting then return end

for element_id, configuration in pairs(setting.value) do
    if configuration.number == parameter_number then
        return setting.id, element_id, configuration
    end
end
