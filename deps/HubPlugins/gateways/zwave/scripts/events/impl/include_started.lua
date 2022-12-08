local params = ...

print("Include started")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event })
