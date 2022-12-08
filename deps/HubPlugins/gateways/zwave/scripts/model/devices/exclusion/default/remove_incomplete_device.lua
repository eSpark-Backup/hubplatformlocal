local self, device_id, node_id = ...

local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

if node_id then
    self:call("remove_node_from_associations", node_id)
    storage:call("delete_table", storage:call("get_monitoring_handler_key", node_id))
    storage:call("remove_device_checking_id", node_id)
end

self:call("clear_device_dependencies", device_id)
core.remove_device(device_id)
