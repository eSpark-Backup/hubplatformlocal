local params = ...

print(
    string.format("Scene activated, node id: %g, channel id: %g, class id: %g, scene id: %g",
        params.node_id, 
        params.channel_id, 
        params.class_id, 
        params.scene_id 
    )
)

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_activated_scene_value", params.node_id, params.channel_id, params.class_id, params.scene_id)
