local params = ...

print(string.format(
    "Association updated, node id: %g, channel id: %g, class id: %g, group id: %g",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.group_id
))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_associations", params.node_id, params.channel_id, params.class_id, params.group_id, params.group_nodes)
