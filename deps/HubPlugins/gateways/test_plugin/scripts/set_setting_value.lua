local params = ...

if not params then
    print("Parameters is empty")
    return
end

local core = require "core"

local setting = assert(core.get_setting(params.setting_id))
if setting.value_type == "action" then
    core.set_setting_value(params.setting_id, setting.value, "pending")
    core.set_setting_value(params.setting_id, setting.value, "synced")
else
    core.set_setting_value(params.setting_id, params.value, "pending")
    core.set_setting_value(params.setting_id, params.value, "synced")
end 
