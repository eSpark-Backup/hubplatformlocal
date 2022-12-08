local params = ...

print(string.format("Value updated, node id: %g, channel id: %g, class id: %g, value: %s, target_value: %s, target_duration: %s",
    params.node_id, params.channel_id, params.class_id, tostring(params.value), tostring(params.target_value), tostring(params.target_duration)
))

print("memory_consumption: ".."value updated 1: " .. collectgarbage("count")*1024)

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)

print("memory_consumption: ".."value updated 2: " .. collectgarbage("count")*1024)

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if params.class_id == class_ids.basic then
    dev_handler:call("update_basic_value", params.node_id, params.channel_id, params.class_id, params.value, params.target_value)
elseif params.class_id == class_ids.dimmer then
    dev_handler:call("update_multilevel_switch", params.node_id, params.channel_id, params.class_id, params.value, params.target_value, params.target_duration)
else
    dev_handler:call("update_value", params.node_id, params.channel_id, params.class_id, params.value, params.target_value, params.target_duration)
end

print("memory_consumption: ".."value updated 3: " .. collectgarbage("count")*1024)
