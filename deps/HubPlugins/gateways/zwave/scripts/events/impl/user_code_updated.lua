local params = ...

print("user codes updated")

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_user_code_value",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.user_code_ranges,
    params.supported_users_count,
    params.user_polling_status
)
