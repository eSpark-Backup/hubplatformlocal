local manufacturer_id, product_type, product_id = ...

local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(manufacturer_id, product_type, product_id)

return  hubplugins.object(dev_handler_id, "HUB:zwave/scripts/model/devices/daemon")
