local params = ...

print("Reconstract network progress for " .. params.current .. "st/th device")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, current = params.current, devices = params.devices })
