local core = assert(require("core"), "Fail to load the 'core' functionality")
local gateway_id = core.get_gateway().id
core.remove_gateway_devices(gateway_id)

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
storage:call("clear")
storage:call("set_storage_version", hubplugins.weak_loadfile("HUB:zwave/scripts/resources/version")())
