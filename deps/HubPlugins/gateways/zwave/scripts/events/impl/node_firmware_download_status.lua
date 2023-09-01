local params = ...

print(string.format("Node firmware download status, image: %s, status: %s, progress: %s, error: %s", params.image, params.status, tostring(params.progress), params.error))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")()
dev_handler:call("handle_node_firmware_download_status", params.image, params.checksum, params.status, params.progress, params.error)

