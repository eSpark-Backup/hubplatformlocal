local self, node_id, channel_id, class_id, value, target_value, duration = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

self:_super_call("default", "update_value", node_id, channel_id, class_id, value, target_value, duration)

-- Note: this device doesn't send the notification when tampering is no longer detected.
-- What it does send, though, is a battery report - so reset the tampering_cover_alarm item here.
if class_id == class_ids.battery then
    self:call("handle_notification", node_id, 0, class_ids.notification, 0x00, 0xFF, "home_security", "state_idle")
end
