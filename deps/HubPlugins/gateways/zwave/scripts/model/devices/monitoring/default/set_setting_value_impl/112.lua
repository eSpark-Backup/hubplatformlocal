local self, setting_id, value, descriptor = ...

local zwave = require("zwave")
local core = require("core")

local setting = core.get_setting(setting_id)

-- Do not update value of read-only setting
if setting.has_setter == false then
    return
end

-- Update value in core preemptively, so that the setting always reflects the desired value
-- Fixme: if a device reports back different value, we'd get 2 boradcasts with different values
if setting.value_type == "action" then
    core.update_setting_status(setting_id, "pending")
else
    core.set_setting_value(setting_id, value, "pending")
end

local new_value = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_configuration_value_from_ui")(setting, value)

zwave.set_configuration_value(descriptor.node_id, setting.info["zwave.parameter_number"], setting.info["zwave.parameter_size"], new_value)

self:call("request_configuration_value", descriptor.node_id, setting.info["zwave.parameter_number"])
