local core = assert(require("core"), "Failed to load core functionality")
local storage = assert(require("storage"), "Failed to load storage functionality")

core.remove_gateway_devices(core.get_gateway().id)

storage.delete_all()
