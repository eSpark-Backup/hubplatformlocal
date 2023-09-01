local self, node_id, interval = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local setting_id = self:call("find_setting_id", node_id, class_ids.wake_up, "wake_up_interval")

if setting_id then
    local core = require("core")
    core.set_setting_value(setting_id, interval, "synced")
end
