local params = ...

print(("Sound switch tone play updated, node id: %g, channel id: %g, class id: %g, tone id: %d, volume: %d"):format(
    params.node_id,
    params.channel_id,
    params.class_id,
    params.tone_id,
    params.volume
))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_sound_switch_tone_play", params.node_id, params.channel_id, params.class_id, params.tone_id, params.volume)
