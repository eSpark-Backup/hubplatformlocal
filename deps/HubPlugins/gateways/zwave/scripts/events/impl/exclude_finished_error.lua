local params = ...

print("Exclude failed")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, error = params.error })
if core.notify_process_stopped then
    core.notify_process_stopped("removing_device", "fail")
end
