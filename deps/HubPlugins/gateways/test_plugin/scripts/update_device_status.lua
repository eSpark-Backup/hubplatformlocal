local params = ... or {}

local core = require "core"

print(string.format("TEST_PLUGIN: Update device status, device_id: %s, status: %s",
        params.device_id, params.status))
core.update_device_status(params.device_id, params.status)
