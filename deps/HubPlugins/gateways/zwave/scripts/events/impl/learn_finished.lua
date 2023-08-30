local params = ...

print("Learning mode finished")

local core = assert(require("core"), "Fail to load the 'core' functionality")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/update_gateway_roles")()

core.send_ui_broadcast({ event = params.event })
