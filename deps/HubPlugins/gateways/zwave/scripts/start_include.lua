local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

require("HUB:zwave/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
if storage:call("should_check_extensions") then
    storage:call("set_check_extensions_flag", false)
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/extensions/process_extensions")()
end

if hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_device_adding_enabled")() then
    zwave.start_include( "HUB:zwave/configs/device_setting.json" )
else
    print( "Controller has no permissions to start include" )
    
    local core = assert(require("core"), "Fail to load the 'core' functionality")
    core.send_ui_broadcast({ event = "include_finished_error", error = { code = -42, data = "hub.gateway.zwave.err.include.perm" } })
end
