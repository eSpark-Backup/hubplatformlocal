local params = ...

print("Learning mode failed")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, error = params.error })
