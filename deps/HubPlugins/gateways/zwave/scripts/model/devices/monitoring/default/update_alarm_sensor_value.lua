local self, node_id, channel_id, class_id, sensor_type, value, delay = ...

local alarm_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/alarm_sensor_map")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local zwave = assert(require("zwave"), "Fail to load the zwave functionality")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

-- main

local item_name = alarm_sensor_map.sensor_type_to_item_name[sensor_type]
local item_tag = alarm_sensor_map.sensor_type_to_item_tag[sensor_type]
local result_value

if item_name == item_names.smoke_alarm then
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

else
    error("please handle following alarm sensor type: " .. tostring(sensor_type))
end

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id, item_tag) or
                self:call("find_item_id", item_name, node_id, channel_id, class_ids.basic) -- support basic CC

assert(item_id, "update_alarm_sensor_value: item id not found")

self:call("update_value_in_core", item_name, item_id, result_value)
