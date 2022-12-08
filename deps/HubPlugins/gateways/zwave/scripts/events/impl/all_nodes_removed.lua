local params = ...

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/clear_storages")()

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/update_gateway_roles")("wizard", "primary")
