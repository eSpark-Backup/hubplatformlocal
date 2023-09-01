local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

require("HUB:zwave/scripts/helpers/scripts_preload")

if hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_device_adding_enabled")() then
    zwave.stop_exclude()
else
    print( "Controller has no permissions to stop exclude" )
end
