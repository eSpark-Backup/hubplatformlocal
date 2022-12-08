local self, setting_id, value, descriptor = ...

if descriptor.tag == "controlled_setpoint_type" then

    local core = require "core"
    local setting = core.get_setting(setting_id)

    if setting.value ~= value and setting.enum[value] then
        core.set_setting_value(setting_id, value, "synced")

        local item_names = require("HUB:zwave/scripts/resources/item_names")
        local class_ids = require("HUB:zwave/scripts/resources/class_ids")

        local thermostat_mode_item = self:call("find_item", item_names.thermostat_mode, descriptor.node_id, 0, class_ids.thermostat_mode)

        self:call("update_thermostat_setpoint_items", descriptor.node_id, 0, thermostat_mode_item and thermostat_mode_item.value, value)
    end
else
    error("Unknown setting descriptor " .. descriptor.tag)
end
