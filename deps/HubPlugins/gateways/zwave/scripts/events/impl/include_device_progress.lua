local params = ...

print("Include device progress " .. params.value)

local core = assert(require("core"), "Fail to load the 'core' functionality")

core.send_ui_broadcast({ event = params.event, value = params.value })
