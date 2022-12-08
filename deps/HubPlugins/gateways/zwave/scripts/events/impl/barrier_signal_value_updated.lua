local params = ...

print(string.format("Barrier signal value updated, node id: %g, channel id: %g, class id: %g, signal name: %s, value: %g", params.node_id, params.channel_id, params.class_id, params.signal_name, params.value))


local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_barrier_signal_value", params.node_id, params.channel_id, params.class_id, params.signal_name, params.value)
