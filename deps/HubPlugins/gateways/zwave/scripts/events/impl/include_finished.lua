local params = ...

print("Include finished")

local broadcasts_repeater = hubplugins.object("broadcasts_repeater", "HUB:zwave/scripts/utils")
broadcasts_repeater:call("stop", "s2_select_authentication_modes")

local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zwave/scripts/helpers")
restrictions_helper:call("include_finished")

local core = assert(require("core"), "Fail to load the 'core' functionality")

core.send_ui_broadcast({ event = "include_device_progress", value = 100 })
core.send_ui_broadcast({ event = params.event })
if core.notify_process_stopped then
    core.notify_process_stopped("adding_device", "success")
end
