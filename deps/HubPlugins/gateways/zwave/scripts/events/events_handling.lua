local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local event_impl_modules_path = "HUB:zwave/scripts/events/impl/"

local handler =
        hubplugins.weak_loadfile(event_impl_modules_path .. params.event)
    or  hubplugins.weak_loadfile(event_impl_modules_path .. "unknown")

return assert(handler)(params)
