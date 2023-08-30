local self, node_id, channel_id, class_id, value, target_value = ...

local water_leak_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/water_leak_alarm_event")

local item_value
if value == 0x00 then -- Norm
    item_value = water_leak_alarm_enum.no_water_leak

elseif value == 0xFF then -- Leak
    item_value = water_leak_alarm_enum.water_leak_detected

else
    error("[132_83_534.update_value]: unsupported sensor binary value: " .. value)
end

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

local item_name = item_names.water_leak_alarm
local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_ids.alarm_sensor, item_tags.water_alarm)

self:call("update_value_in_core", item_name, item_id, item_value)
