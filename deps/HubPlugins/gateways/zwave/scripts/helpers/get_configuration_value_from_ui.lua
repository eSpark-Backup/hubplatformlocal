local setting, value = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local value_mapping = storage:call("find_enum_value_mapping", setting.id)
local converter = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/setting_value_converter")(value_mapping)

local result = converter.from_ui(value, setting.value_type)

if not result then
    error(("%s is not a valid %s value"):format(hubplugins.dump(value), setting.value_type))
end

return result
