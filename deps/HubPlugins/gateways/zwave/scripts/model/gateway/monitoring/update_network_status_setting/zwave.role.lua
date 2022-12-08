local self, info = ...

local template = "Suc SIS: %s PRI: %s"

local function bool_to_string(value)
    return value == true and "YES" or "NO"
end

local new_value = template:format(
    bool_to_string(info.roles.sis), 
    bool_to_string(info.roles.primary)
)

self:call("set_setting_value_to_core", "zwave.role", new_value)
