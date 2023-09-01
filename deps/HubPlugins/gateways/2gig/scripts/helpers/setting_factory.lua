local setting_factory = {}

function setting_factory.create_setting(type, info)
    return setting_factory['create_' .. type .. '_setting'](info)
end

function setting_factory.create_base_setting(info)
    return {
        label = {
            text = info.label,
            lang_tag = info.tag .. "_label"
        },
        description = {
            text = info.description,
            lang_tag = info.tag .. "_description"
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

function setting_factory.create_token_setting(info)
    local setting = setting_factory.create_base_setting(info)

    setting.value_type = "token"
    setting.enum = {}
    for key, value in pairs(info.enum) do
        setting.enum[key] = {
            text = value,
            lang_tag = info.tag .. "_" .. key .. "_label"
        }
    end

    return setting
end

return setting_factory
