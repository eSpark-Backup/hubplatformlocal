local params = ...

print("Action busy")
local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, action_type = params.action_type })
