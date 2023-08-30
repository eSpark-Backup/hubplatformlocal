local self, setting_id, value = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local descriptor = storage:call("find_setting_descriptor", setting_id)

local filename = descriptor.class_id and math.floor(descriptor.class_id) or "default"
self:call("set_setting_value_impl/" .. filename, 
            setting_id, 
            value, 
            descriptor)
