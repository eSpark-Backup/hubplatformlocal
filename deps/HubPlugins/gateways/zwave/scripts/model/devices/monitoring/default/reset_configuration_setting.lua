local self, setting_id, node_id, should_reset_configuration = ...

local core = require("core")

local setting = core.get_setting(setting_id)

if setting.info ~= nil then
    local parameter_number = setting.info["zwave.parameter_number"]
    local parameter_size = setting.info["zwave.parameter_size"]

    local value
    if setting.value_default ~= nil then
        value = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_configuration_value_from_ui")(setting, setting.value_default)
    end

    if should_reset_configuration(parameter_number) and setting.has_setter ~= false then
        self:call("reset_configuration_value", 0, node_id, parameter_number, parameter_size, value)
    end
elseif setting.value_type == "dictionary.zwave_configuration" then
    local delay = 0
    for _, configuration in pairs(setting.value) do
        if should_reset_configuration(configuration.number) and configuration.has_setter ~= false then
            delay = self:call("reset_configuration_value", delay, node_id, configuration.number, configuration.size)
        end
    end
end
