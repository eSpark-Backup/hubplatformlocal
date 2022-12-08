local params = ...

print(string.format("Door lock state updated, node id: %g, channel id: %g", params.node_id, params.channel_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_door_lock_state", params.node_id, params.channel_id, params.door_lock_mode, params.door_condition)
