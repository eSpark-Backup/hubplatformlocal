local self, node_id, class_id, tag = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local setting_id = storage:call("find_setting_id", node_id, class_id, tag)

if setting_id ~= nil then
    return setting_id
else
    print(string.format("[handler.find_setting_id]: Setting id not found: node id: %g, class id: %s, parameter_number: %s",
            node_id,
            class_id or 0,
            tostring(tag)))
end
