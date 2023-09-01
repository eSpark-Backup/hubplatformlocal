local self, zwave_node_id = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

return {
    {
        interval = 60000,
        id = self:call("_get_timer_id", zwave_node_id),
        script = "HUB:zwave/scripts/timer_handlers/common/request_value",
        script_params = {
            node_id = zwave_node_id,
            channel_id = 0,
            class_id = class_ids.meter,
        }
    }
}
