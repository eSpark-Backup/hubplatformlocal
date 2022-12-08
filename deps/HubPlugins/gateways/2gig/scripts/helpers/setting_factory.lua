local setting_factory = {}

function setting_factory.create_setting(type, info)
    return setting_factory['create_' .. type .. '_setting'](info)
end

function setting_factory.create_base_setting(info)
    return {
        label = {
            lang_tag = info.label,
            text = info.tag .. "_label"
        },
        description = {
            lang_tag = info.description,
            text = info.tag .. "_description"
        },
        info = {
            tag = info.tag
        },
        value = info.default,
        value_default = info.default,
        has_setter = true
    }
end

function setting_factory.create_number_setting(info)
    local setting = setting_factory.create_base_setting(info)

    setting.value_type = "int"
    setting.value_min = info.min
    setting.value_max = info.max

    return setting
end

return setting_factory
