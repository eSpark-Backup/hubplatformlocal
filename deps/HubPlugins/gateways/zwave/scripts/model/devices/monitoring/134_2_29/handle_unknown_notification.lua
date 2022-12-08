local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, raw_value = ...

local tampering_cover_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_cover_events")

local alarm_tamper = 0x00

local item_value
if v1_alarm_type == alarm_tamper then
    if v1_alarm_level == 0x00 then -- Close
        item_value = tampering_cover_enum.no_tampering_cover

    elseif v1_alarm_level == 0xFF then -- Open
        item_value = tampering_cover_enum.tampering_cover

    else
        error("[134_2_29.handle_unknown_notification]: unsupported notification value: " .. v1_alarm_level)
    end

else
    error("[134_2_29.handle_unknown_notification]: unsupported notification received: " .. v1_alarm_type)
end

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_name = item_names.tampering_cover_alarm
local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)

self:call("update_value_in_core", item_name, item_id, item_value)
