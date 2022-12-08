local params = ...

local base_path = "HUB:2gig/scripts/events/impl/"

local handler = loadfile(base_path .. params.event)
             or loadfile(base_path .. "unknown")

return assert(handler)(params)
