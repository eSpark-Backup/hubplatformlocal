local core = require("core")
local device = loadfile("HUB:siren/scripts/device/siren")()

local dev_id = core.add_device(device)
assert(dev_id, "Failed to create siren device")

return dev_id
