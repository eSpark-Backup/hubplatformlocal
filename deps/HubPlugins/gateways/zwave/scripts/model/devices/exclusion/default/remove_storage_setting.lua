local self, setting_id = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local descriptor = storage:call("find_setting_descriptor", setting_id)

if descriptor then
    if descriptor.class_id == class_ids.configuration then
        storage:call("remove_enum_value_mapping", setting_id)
    elseif descriptor.class_id == class_ids.anti_theft then
        storage:call("remove_value", storage:call("get_anti_theft_hint_key", descriptor.node_id))
    end
    storage:call("remove_setting_mapping", setting_id)
else
    print("Settings descriptor not found for " .. setting_id)
end
