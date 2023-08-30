local self, node_id, channel_id, class_id, parameter_number, parameter_size, value = ...

local core = assert(require("core"), "Failed to load core functionality")

local tag = tostring(math.floor(parameter_number))

do
    local setting = self:call("find_setting", node_id, class_id, tag)

    if setting ~= nil and setting.value_type ~= "action" then
        local new_value = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_setting_value_from_configuration")(setting, value)

        core.set_setting_value(setting.id, new_value, "synced")
    end
end

do
    local setting_id, element_id, configuration = self:call("find_parameter_in_settings_dictionary", node_id, parameter_number)

    if element_id ~= nil then
        core.set_setting_dictionary_value(setting_id, element_id, {
            name = configuration.name,
            description = configuration.description,
            number = configuration.number,
            size = configuration.size,
            value = value,
            format = configuration.format,
            hasSetter = configuration.hasSetter,
            status = "synced"
        })
    end
end

self:call("handle_configuration_update", node_id, parameter_number, value)
