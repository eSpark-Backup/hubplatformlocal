local zwave_node = ...
local security_class_started, security = zwave_node.security_class_started[1], zwave_node.security[1]


--main
print("Try to calculate security: security = " .. security .. ", security_class_started = " .. security_class_started)

local zwave_security_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/zwave_security_flag")
local security_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/security_flag")


local function has_s2()
    local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
    local s2_id = require("HUB:zwave/scripts/resources/class_ids").s2
    local _, root_channel = find_if(zwave_node.channels, function(channel) return channel.id == 0 end)
    return root_channel and root_channel.classes and find_if(root_channel.classes, function(cc) return cc.id == s2_id end) ~= nil
end

if security_class_started == zwave_security_enum.none or 
    security == zwave_security_enum.none or 
    ( --[[certification requirements]] security_class_started == zwave_security_enum.s0 and has_s2() ) then
    return security_enum.no
elseif security_class_started == zwave_security_enum.authenticated or security_class_started == zwave_security_enum.access_control then
    if security == zwave_security_enum.authenticated or security == zwave_security_enum.access_control then
        return security_enum.high
    else
        return security_enum.low
    end
elseif security_class_started == zwave_security_enum.unauthenticated then
    if security_class_started == security then
        return security_enum.middle
    else
        return security_enum.low
    end
elseif security_class_started == zwave_security_enum.s0 then
    if security_class_started == security then
        return security_enum.low
    end
end
