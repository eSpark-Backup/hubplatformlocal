local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, raw_value = ...

print("Handle notification from 316_2_80")

if notification_type == 0x07 then
    local item_names = require("HUB:zwave/scripts/resources/item_names")
    local notification_type =  require("HUB:zwave/scripts/resources/item_descriptor_tags").home_security
    local notification_mappings = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/" .. notification_type)

    local item_id = self:call("find_item_id", item_names.motion, node_id, channel_id, class_id, notification_mappings.item_tag)
    self:call("update_value_in_core", item_names.motion, item_id, false)
else
    print("Unsupported notification received")
end
