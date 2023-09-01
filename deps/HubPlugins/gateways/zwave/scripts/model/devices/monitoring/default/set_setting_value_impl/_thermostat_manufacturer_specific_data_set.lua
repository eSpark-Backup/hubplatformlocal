local self, setting_id, value, descriptor = ...

local is_request_value_needed = false

if descriptor.tag == "thermostat_mode_manufacturer_specific_data" then
    local core = require "core"

    local item_names = require("HUB:zwave/scripts/resources/item_names")
    local thermostat_mode_item = self:call("find_item", item_names.thermostat_mode, descriptor.node_id, descriptor.channel_id, descriptor.class_id)
    local mode = thermostat_mode_item and thermostat_mode_item.value

    if mode and mode == "manufacturer_specific" then
        local settings_fail_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", setting_id)
        settings_fail_timer:call("start")

        core.set_setting_value(setting_id, value, "pending")

        local zwave = require "zwave"
        local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")

        zwave.set_thermostat_mode(descriptor.node_id, descriptor.channel_id, mode_enum.token_to_value["manufacturer_specific"], value)

        is_request_value_needed = true
    else
        core.set_setting_value(setting_id, value, "synced")
    end
else
    error("[set_setting.value.64] Unexpected setting tag " .. descriptor.tag)
end

return is_request_value_needed
