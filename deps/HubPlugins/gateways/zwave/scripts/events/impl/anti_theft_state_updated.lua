local params = ...

print(("anti_theft_state_updated. node id: %g, channel id: %g, class id: %g, locked: %s"), 
       params.node_id, params.channel_id, params.class_id, params.lock_info and "yes" or "no")

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_anti_theft_state", params.node_id, params.channel_id, params.class_id, params.lock_info)
