local params = ...

print("Include invoked")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event })
if core.notify_process_started then
    core.notify_process_started("adding_device")
end
