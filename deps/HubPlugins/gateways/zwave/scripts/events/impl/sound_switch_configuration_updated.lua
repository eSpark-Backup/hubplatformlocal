local params = ...

print(("Sound switch configuration updated, node id: %g, channel id: %g, class id: %g, volume: %d, default tone id: %d"):format(
    params.node_id,
    params.channel_id,
    params.class_id,
    params.volume,
    params.default_tone_id
))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_sound_switch_configuration", params.node_id, params.channel_id, params.class_id, params.volume, params.default_tone_id)
