local self, node_id, channel_id, override_type, mode, value = ...

if mode == "unknown" then
    print("thermostat setback mode is unknown. How to handle it?")
    value = 0
end

local core = require "core"

local thermostat_setback_cc_id = require("HUB:zwave/scripts/resources/class_ids").thermostat_setback
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require "core"

local function set_setting_value(id, value)
    if id then
        hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", id):call("stop")
        core.set_setting_value(id, value, "synced")
    end
end

set_setting_value(storage:call("find_setting_id", node_id, thermostat_setback_cc_id, "thermostat_setback_mode"), mode)

local value_setting_id = storage:call("find_setting_id", node_id, thermostat_setback_cc_id, "thermostat_setback_value")  
if value_setting_id then
    if value and mode == "custom_value" or mode == "unknown" then
        set_setting_value(value_setting_id, { value = value, scale = "celsius", relative = true })
    else
        hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", value_setting_id):call("stop")
        core.update_setting_status(value_setting_id, "synced")
    end
end

set_setting_value(storage:call("find_setting_id", node_id, thermostat_setback_cc_id, "thermostat_setback_override_type"), override_type)
