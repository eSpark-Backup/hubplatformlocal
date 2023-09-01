local params = ...

print(string.format("Year day entry lock value updated, node id: %g, channel id: %g, class id: %g, user id: %g, slot id: %g", 
    params.node_id, params.channel_id, params.class_id, params.user_id, params.slot_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("year_day_entry_lock_schedule_updated", params.node_id, params.channel_id, params.class_id, params.user_id, params.slot_id, params.value)