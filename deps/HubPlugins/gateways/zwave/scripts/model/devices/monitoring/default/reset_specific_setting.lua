local self, setting_id, descriptor = ...

local core = require("core")

local setting = core.get_setting(setting_id)

local is_container_value_type = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_container_value_type")

local value
-- default
if setting.value_default ~= nil then
    value = setting.value_default

-- types
elseif setting.value_type == "int" or setting.value_type == "float" then
    value = 0

elseif setting.value_type == "bool" then
    value = true

elseif setting.value_type == "token" and setting.enum then
    for token in pairs(setting.enum) do
        if token ~= "unknown" and (value == nil or tostring(token) < value) then
            value = tostring(token)
        end
    end

elseif is_container_value_type(setting.value_type) then
    value = {}

elseif type(setting.value) == "table" and setting.value.scale --[[ scalable ]] then
    value = setting.value
    value.value = 0

elseif setting.value_type == "string" then
    value = ""

else
    print("Cannot detect default value of setting type " .. setting.value_type)
end

if value then
    self:call("set_setting_value", setting_id, value)
end
