local self, setting_id, value, descriptor = ...

if descriptor.tag == "thermostat_setback_value" then
    local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")
    value.value = unit_converter:call( "convert_offset", value.value, value.scale, "celsius" )

    local min_value = -12.8
    local max_value = 12
    value.value = math.max(math.min(value.value, max_value), min_value)
    value.scale = "celsius"
    value.relative = true
end

local thermostat_setback_timer = hubplugins.object("thermostat_setback_timer", "HUB:zwave/scripts/timer_handlers", descriptor.node_id, 0)
thermostat_setback_timer:call("start", descriptor.tag, value)

local settings_fail_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", setting_id)
settings_fail_timer:call("start")

local core = require "core"

core.set_setting_value(setting_id, value, "pending")
