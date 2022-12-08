local params = ...

print("Learning mode timeout")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, error = params.error })
