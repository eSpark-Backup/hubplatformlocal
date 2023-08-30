local params = ...

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call(
    "update_simple_av_control_activated",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.key_attribute,
    params.commands
)
