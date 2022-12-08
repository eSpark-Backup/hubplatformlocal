local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, raw_value = ...

print("Handle notification from 312_1_2")

local smoke_alarm = 0x01
local co_alarm = 0x02
local alarm_test = 0x0C

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_name = ""
local item_value = ""

local smoke_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/smoke_alarm_event")
local co_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co_alarm_event")
local test_state_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/test_state")

if v1_alarm_type == smoke_alarm then
    item_name = item_names.smoke_alarm
    if v1_alarm_level > 0 then
        item_value = smoke_alarm_enum.smoke_detected
    elseif v1_alarm_level == 0 then
        item_value = smoke_alarm_enum.no_smoke
    end

elseif v1_alarm_type == co_alarm then
    item_name = item_names.co_alarm
    if v1_alarm_level > 0 then
        item_value = co_alarm_enum.co_detected
    else
        item_value = co_alarm_enum.no_co
    end

elseif v1_alarm_type == alarm_test then
    item_name = item_names.test_state
    if v1_alarm_level > 0 then
        item_value = test_state_enum.test_ok
    else
        item_value = test_state_enum.no_test
    end

else
    print("Unsupported notification received")
end

if item_name ~= "" and item_value ~= "" then
    local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
    self:call("update_value_in_core", item_name, item_id, item_value)
end
