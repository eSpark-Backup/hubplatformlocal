local params = ...

print("Include failed")

local broadcasts_repeater = hubplugins.object("broadcasts_repeater", "HUB:zwave/scripts/utils")
broadcasts_repeater:call("stop", "s2_select_authentication_modes")

local core = assert(require("core"), "Fail to load the 'core' functionality")
core.send_ui_broadcast({ event = params.event, error = params.error })
if core.notify_process_stopped then
    core.notify_process_stopped("adding_device", "fail")
end
