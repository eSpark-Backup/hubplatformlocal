local manual_device_adding, role = ...

local core = assert(require("core"), "Fail to load the 'core' functionality")
local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

if not manual_device_adding or not role then
    local info = zwave.get_info()
    
    if not manual_device_adding then
        manual_device_adding = (info.roles.inclusion or info.roles.primary or info.roles.sis) and "wizard" or "no"
    end
    
    if not role then
        role = (info.roles.primary or info.roles.sis) and "primary" or "secondary"
    end
end

core.update_gateway({
    manual_device_adding = manual_device_adding,
    info = {
        ["zwave.role"] = role
    }
})
