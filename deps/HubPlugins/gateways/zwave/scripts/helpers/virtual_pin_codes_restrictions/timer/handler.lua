require("HUB:zwave/scripts/helpers/scripts_preload")

local core = require("core")
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )
local time_parser = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/parse_time_string")
local less_or_equal = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/less_or_equal")

local restriction_event_time = storage:call("find_nearest_restriction_event_time")

if not restriction_event_time then
    print("No next virtual zwave pin codes restriction event expected, ignoring timer")
    return
end

local current_time = time_parser(core.get_local_time())

if less_or_equal(restriction_event_time, current_time) then
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")()
end
