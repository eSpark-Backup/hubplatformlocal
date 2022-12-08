local params = ...

print("Include device")

local core = assert(require("core"), "Fail to load the 'core' functionality")

if params.status ~= "finished" then
    core.send_ui_broadcast({ event = params.event, status = params.status })
end

if params.status == "invoked" then
    if core.notify_process_started then
        core.notify_process_started("adding_device")
    end
elseif params.status == "finished" then
    if core.notify_process_stopped then
        core.notify_process_stopped("adding_device", "success")
    end
elseif params.status == "failed" then
    if core.notify_process_stopped then
        core.notify_process_stopped("adding_device", "fail")
    end
elseif params.status == "timeout" then
    if core.notify_process_stopped then
        core.notify_process_stopped("adding_device", "fail")
    end
end
