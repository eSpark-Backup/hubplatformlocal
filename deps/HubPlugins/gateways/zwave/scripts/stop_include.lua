local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

require("HUB:zwave/scripts/helpers/scripts_preload")

local broadcasts_repeater = hubplugins.object("broadcasts_repeater", "HUB:zwave/scripts/utils")
broadcasts_repeater:call("stop", "s2_select_authentication_modes")

if hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_device_adding_enabled")() then
    zwave.stop_include()
else
    print( "Controller has no permissions to stop include" )
end
