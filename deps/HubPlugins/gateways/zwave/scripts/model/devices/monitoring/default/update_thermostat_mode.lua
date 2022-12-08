local self, node_id, channel_id, class_id, mode, manufacturer_specific_data = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_id = self:call("find_item_id", item_names.thermostat_mode, node_id, channel_id, class_id)
assert(item_id,"[handler.update_thermostat_mode]: " ..
        self:call("create_item_id_not_found_error_message", item_names.thermostat_mode, node_id, channel_id, class_id))

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")
local token_value = mode_enum.value_to_token[mode]

local thermostat_mode_update_timer = hubplugins.object("thermostat_mode_update_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id)
thermostat_mode_update_timer:call("stop")

local thermostat_setpoint_set_timer = hubplugins.object("thermostat_setpoint_set_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id)
if not thermostat_setpoint_set_timer:call("exists") then
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")
    local setpoint_type_setting = self:call("find_setting", node_id, class_ids.thermostat_setpoint, "controlled_setpoint_type")
    self:call("update_thermostat_setpoint_items", node_id, channel_id, token_value, setpoint_type_setting and setpoint_type_setting.value)
end

self:call("update_value_in_core", item_names.thermostat_mode, item_id, token_value)

if token_value == "manufacturer_specific" and hubplugins.device_settings_enabled then
    local setting_id = self:call("find_setting_id", node_id, class_id, "thermostat_mode_manufacturer_specific_data")
    if setting_id then
        local setting_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", setting_id)
        setting_timer:call("stop")
        local core = require("core")
        core.set_setting_value(setting_id, manufacturer_specific_data, "synced")
    end
end

local energy_saving_mode_handler = hubplugins.object("energy_saving_mode", "HUB:zwave/scripts/helpers")
energy_saving_mode_handler:call("init_mode_changed_timer")
