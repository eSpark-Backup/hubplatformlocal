local setting, value = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local value_mapping = storage:call("find_enum_value_mapping", setting.id)
local converter = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/setting_value_converter")(value_mapping)

if type(setting.value) == "table" and setting.value.scale ~= nil then
    return converter.to_ui(value, setting.value_type, setting.value.scale)
else
    return converter.to_ui(value, setting.value_type)
end
