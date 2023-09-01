local params = ...

print("Reconstract network error. Node ID " .. params.node_id)

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, node_id = params.node_id, error = params.error })
