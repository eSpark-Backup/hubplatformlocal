local params = ...

print("Include device")

local core = assert(require("core"), "Fail to load the 'core' functionality")

if params.status ~= "finished" then
    core.send_ui_broadcast({ event = params.event, status = params.status })
end

local should_reset_restrictions = false
if params.status == "invoked" then
    if core.notify_process_started then
        core.notify_process_started("adding_device")
    end
elseif params.status == "finished" then
    if core.notify_process_stopped then
        core.notify_process_stopped("adding_device", "success")
    end
    should_reset_restrictions = true
elseif params.status == "failed" then
    if core.notify_process_stopped then
        core.notify_process_stopped("adding_device", "fail")
    end
    should_reset_restrictions = true
elseif params.status == "timeout" then
    if core.notify_process_stopped then
        core.notify_process_stopped("adding_device", "fail")
    end
    should_reset_restrictions = true
end

if should_reset_restrictions then
    local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
    restrictions_helper:call("include_finished")
end
