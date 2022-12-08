local node_id, manufacturer_id, product_type, product_id = ...

local daemon_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_daemon")(manufacturer_id, product_type, product_id)

if daemon_handler then
    daemon_handler:call("stop", node_id)
end
