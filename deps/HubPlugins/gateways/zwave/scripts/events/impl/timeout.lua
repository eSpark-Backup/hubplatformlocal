local params = ...

print("Operation timeout")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event })
