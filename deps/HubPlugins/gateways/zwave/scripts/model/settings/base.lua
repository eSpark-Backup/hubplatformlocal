local node_id, dev_handler_id, setting_info, value_mapping = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local setting = {
    label = {
        lang_tag = setting_info.name_tag or table.concat({ "zwave_config_label", dev_handler_id, setting_info.configuration.number }, "_"),
        text = setting_info.name
    },
    description = {
        lang_tag = setting_info.description_tag or table.concat({ "zwave_config_descr", dev_handler_id, setting_info.configuration.number }, "_"),
        text = setting_info.description
    },
    value_type = setting_info.type,
    enum = setting_info.enum,
    value_mapping = value_mapping,
    info = {
        ["zwave.parameter_number"] = setting_info.configuration.number,
        ["zwave.parameter_size"] = setting_info.configuration.size
    },
    descriptor = {
        node_id = node_id,
        class_id = class_ids.configuration,
        tag = tostring(math.floor(setting_info.configuration.number))
    },
    has_setter = setting_info.has_setter
}

local converter = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/setting_value_converter")(value_mapping)

if setting_info.configuration.value ~= nil then
    setting.value_default = converter.to_ui(setting_info.configuration.value, setting_info.type, setting_info.scale)
    setting.value = setting.value_default
else
    setting.value = converter.get_default_value(setting_info.type, setting_info.scale)
end

if setting_info.value_min ~= nil then
    setting.value_min = converter.to_ui(setting_info.value_min, setting_info.type, setting_info.scale)
end

if setting_info.value_max ~= nil then
    setting.value_max = converter.to_ui(setting_info.value_max, setting_info.type, setting_info.scale)
end

return setting
