local params = ...

if not params then
    print("Parameters is empty")
    return
end

local core = require "core"

local setting = assert(core.get_setting(params.setting_id))

if params.value ~= nil then
    params.value.status = "pending"
    core.set_setting_dictionary_value(params.setting_id, params.key, params.value)
    
    params.value.status = "synced"
    if not params.value.value then
        params.value.value = 0
    end
    core.set_setting_dictionary_value(params.setting_id, params.key, params.value)
else
    core.set_setting_dictionary_value(params.setting_id, params.key, {})
end
