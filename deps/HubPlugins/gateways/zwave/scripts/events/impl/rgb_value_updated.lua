local params = ...

print(string.format( "Rgb value updated, node id: %g, channel id: %g", params.node_id, params.channel_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_rgb_value", params.node_id, params.channel_id, params.colors, params.target_colors, params.target_durations)
