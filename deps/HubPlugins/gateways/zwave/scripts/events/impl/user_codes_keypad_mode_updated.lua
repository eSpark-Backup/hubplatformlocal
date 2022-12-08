local params = ...

print(string.format("User codes keypad mode updated, node id: %g, channel id: %g, class id: %g, mode: %s", params.node_id, params.channel_id, params.class_id, params.value))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_user_codes_keypad_mode", params.node_id, params.channel_id, params.class_id, params.value)
