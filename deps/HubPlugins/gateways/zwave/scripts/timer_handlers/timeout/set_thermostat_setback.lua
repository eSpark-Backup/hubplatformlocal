local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

print(("timer: set_thermostat_setback values. override type: %s, mode: %s, value: %s"):format(
    params.thermostat_setback_override_type, 
    params.thermostat_setback_mode, 
    tostring(params.thermostat_setback_value and params.thermostat_setback_value.value)))

local zwave = require "zwave"
local core = require "core"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local thermostat_setback_cc_id = require("HUB:zwave/scripts/resources/class_ids").thermostat_setback

local value = params.thermostat_setback_value
local mode = params.thermostat_setback_mode
local override_type = params.thermostat_setback_override_type

local value_setting_id = storage:call("find_setting_id", params.node_id, thermostat_setback_cc_id, "thermostat_setback_value")
assert(value_setting_id)
local value_setting = core.get_setting(value_setting_id)
assert(value_setting)

if value == nil then
    value = value_setting.value
end
value.relative = true

if mode == nil then
    local setting_id = storage:call("find_setting_id", params.node_id, thermostat_setback_cc_id, "thermostat_setback_mode")
    local setting = setting_id and core.get_setting(setting_id)
    mode = setting and setting.value or "custom_value"
end

mode = mode == "unknown" and "custom_value" or mode

if override_type == nil then
    local setting_id = storage:call("find_setting_id", params.node_id, thermostat_setback_cc_id, "thermostat_setback_override_type")
    local setting = setting_id and core.get_setting(setting_id)
    override_type = setting and setting.value == "temporary_override" and setting.value or "permanent_override"
end

override_type = override_type == "unknown" and "permanent_override" or override_type

local zwave_value

if mode == "custom_value" then
    local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")
    zwave_value = unit_converter:call( "convert_offset", value.value, value.scale, "celsius" )
end

if params.thermostat_setback_mode or params.thermostat_setback_override_type or mode == "custom_value" or mode == "unknown" then
    print(("timer: set_thermostat_setback values for setting to zwave. override type: %s, mode: %s, value: %s"):format(override_type, mode, tostring(zwave_value)))

    zwave.set_thermostat_setback_value(params.node_id, params.channel_id, override_type, mode, zwave_value)
    zwave.request_value(params.node_id, thermostat_setback_cc_id, params.channel_id)
else
    print(("timer: set_thermostat_setback, set setting value to core. value: %g, scale: %s, relative: %s"):format(value.value, value.scale, tostring(value.relative)))
    core.set_setting_value(value_setting_id, value, "synced")
end
