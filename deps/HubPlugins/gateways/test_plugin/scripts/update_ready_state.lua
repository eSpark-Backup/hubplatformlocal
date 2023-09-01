local params = ... or {}

local core = require "core"

print(string.format("TEST_PLUGIN: Update ready state, device_id: %s, ready: %s",
        params.device_id, tostring(params.ready)))
core.update_ready_state(params.device_id, params.ready)
