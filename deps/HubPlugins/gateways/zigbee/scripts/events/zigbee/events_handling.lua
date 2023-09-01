local params = ...

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local event_impl_modules_path = "HUB:zigbee/scripts/events/zigbee/"

local handler =
        hubplugins.weak_loadfile(event_impl_modules_path .. params.event)
    or  hubplugins.weak_loadfile(event_impl_modules_path .. "unknown")

return assert(handler)(params)
