local self, node_id, parameter_number, status = ...

local core = assert(require("core"), "Failed to load core functionality")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_id = require("HUB:zwave/scripts/resources/class_ids").configuration

local tag = tostring(math.floor(parameter_number))

do
    local setting_id = self:call("find_setting_id", node_id, class_id, tag)

    if setting_id ~= nil then
        core.update_setting_status(setting_id, status)
    end
end

do
    local setting_id, element_id = self:call("find_parameter_in_settings_dictionary", node_id, parameter_number)

    if element_id ~= nil then
        core.update_setting_dictionary_status(setting_id, element_id, status)
    end
end
