local self, node_id, channel_id, class_id, sensor_type, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local binary_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/binary_sensor_map")
local item_name = binary_sensor_map.sensor_type_to_item_name[sensor_type]
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if (not item_name) or (item_name == item_names.sensor_binary) then
    local zwave = require("zwave")
    local zwave_node = zwave.get_node_info(node_id)

    local cc_data = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_basic_cc_data_by_device_info")(zwave_node)
    self:call("handle_cc_data", cc_data, node_id, channel_id, value)
    return
end

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id, binary_sensor_map.sensor_type_to_item_tag[sensor_type]) or
                self:call("find_item_id", item_name, node_id, channel_id, class_ids.basic) -- support basic CC
assert( item_id,  "[handler.update_binary_sensor_value]: " .. self:call(
    "create_item_id_not_found_error_message", 
    item_name, 
    node_id, 
    channel_id, 
    class_id, 
    binary_sensor_map.sensor_type_to_item_tag[sensor_type])
)

local result_value
if item_name == item_names.motion or item_name == item_names.aux_binary then
    result_value = value ~= 0

elseif item_name == item_names.smoke_alarm then
    local smoke_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/smoke_alarm_event")
    result_value = value == 0x00 and smoke_alarm_enum.no_smoke or smoke_alarm_enum.smoke_detected

elseif item_name == item_names.co_alarm then
    local co_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co_alarm_event")
    result_value = value == 0x00 and co_alarm_enum.no_co or co_alarm_enum.co_detected

elseif item_name == item_names.co2_alarm then
    local co2_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co2_alarm_event")
    result_value = value == 0x00 and co2_alarm_enum.no_co2 or co2_alarm_enum.co2_detected

elseif item_name == item_names.heat_alarm then
    local heat_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/heat_alarm_event")
    result_value = value == 0x00 and heat_alarm_enum.heat_ok or heat_alarm_enum.overheat_detected

elseif item_name == item_names.water_leak_alarm then
    local water_leak_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/water_leak_alarm_event")
    result_value = value == 0x00 and water_leak_alarm_enum.no_water_leak or water_leak_alarm_enum.water_leak_detected

elseif item_name == item_names.freeze_alarm then
    local freeze_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/freeze_alarm_event")
    result_value = value == 0x00 and freeze_alarm_enum.no_freeze or freeze_alarm_enum.freeze_detected

elseif item_name == item_names.tampering_cover_alarm then
    local tampering_cover_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_cover_events")
    result_value = value == 0x00 and tampering_cover_enum.no_tampering_cover or tampering_cover_enum.tampering_cover

elseif item_name == item_names.tilt then
    local tilt_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tilt_event")
    result_value = value == 0x00 and tilt_enum.no_tilt or tilt_enum.tilt

elseif item_name == item_names.dw_state then
    local dw_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/dw_state_event")
    result_value = value == 0x00 and dw_enum.dw_is_closed or dw_enum.dw_is_opened

elseif item_name == item_names.glass_breakage_alarm then
    local glass_breakage_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/glass_breakage_event")
    result_value = value == 0x00 and glass_breakage_enum.no_glass_breakage or glass_breakage_enum.glass_breakage

else
    error("update unknown binary sensor: " .. tostring(sensor_type))
end

self:call("update_value_in_core", item_name, item_id, result_value)
