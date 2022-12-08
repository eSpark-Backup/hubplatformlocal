local self, inclusion_context, build_tree_context, device_node, sensor_type = ...

print(("[default.48.get_item] processing of sensor type: 0x%02X"):format(sensor_type))

local item_names = require("HUB:zwave/scripts/resources/item_names")
local binary_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/binary_sensor_map")

local item_name = binary_sensor_map.sensor_type_to_item_name[sensor_type]

if item_name == item_names.sensor_binary then
    return nil
end

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_name, binary_sensor_map.sensor_type_to_item_tag[sensor_type])
local append_to_table = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")

local append_to_item_enum = function(...--[[tokens]])
    append_to_table(item.enum, ...)
end


if item_name == item_names.smoke_alarm then
    local smoke_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/smoke_alarm_event")
    append_to_item_enum(
        smoke_alarm_enum.no_smoke,
        smoke_alarm_enum.smoke_detected
    )

elseif item_name == item_names.co_alarm then
    local co_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co_alarm_event")
    append_to_item_enum(
        co_alarm_enum.no_co,
        co_alarm_enum.co_detected
    )

elseif item_name == item_names.co2_alarm then
    local co2_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co2_alarm_event")
    append_to_item_enum(
        co2_alarm_enum.no_co2,
        co2_alarm_enum.co2_detected
    )

elseif item_name == item_names.heat_alarm then
    local heat_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/heat_alarm_event")
    append_to_item_enum(
        heat_alarm_enum.heat_ok,
        heat_alarm_enum.overheat_detected
    )

elseif item_name == item_names.water_leak_alarm then
    local water_leak_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/water_leak_alarm_event")
    append_to_item_enum(
        water_leak_alarm_enum.no_water_leak,
        water_leak_alarm_enum.water_leak_detected
    )

elseif item_name == item_names.freeze_alarm then
    local freeze_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/freeze_alarm_event")
    append_to_item_enum(
        freeze_alarm_enum.no_freeze,
        freeze_alarm_enum.freeze_detected
    )

elseif item_name == item_names.dw_state then
    local dw_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/dw_state_event")
    append_to_item_enum(
        dw_enum.dw_is_closed,
        dw_enum.dw_is_opened
    )

elseif item_name == item_names.tampering_cover_alarm then
    local tampering_cover_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_cover_events")
    append_to_item_enum(
        tampering_cover_enum.no_tampering_cover,
        tampering_cover_enum.tampering_cover
    )

elseif item_name == item_names.glass_breakage_alarm then
    local glass_breakage_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/glass_breakage_event")
    append_to_item_enum(
        glass_breakage_enum.no_glass_breakage,
        glass_breakage_enum.glass_breakage
    )

elseif item_name == item_names.tilt then
    local tilt_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tilt_event")
    append_to_item_enum(
        tilt_enum.no_tilt,
        tilt_enum.tilt
    )
end

return item
