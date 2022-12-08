local params = ...

local event_impl_modules_path = "HUB:cloud_devices/scripts/events/"

local handler =
        loadfile(event_impl_modules_path .. params.event)
    or  loadfile(event_impl_modules_path .. "unknown")

return assert(handler)(params)
