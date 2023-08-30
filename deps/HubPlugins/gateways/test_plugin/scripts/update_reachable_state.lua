local params = ... or {}

local core = require "core"

print(string.format("TEST_PLUGIN: Update reachable state, device_id: %s, reachable: %s",
        params.device_id, tostring(params.reachable)))
core.update_reachable_state(params.device_id, params.reachable)
