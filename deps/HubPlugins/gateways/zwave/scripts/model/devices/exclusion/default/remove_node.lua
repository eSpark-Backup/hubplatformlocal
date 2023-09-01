local self, node_id = ...

local core = require("core")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

self:call("remove_node_from_associations", node_id)

local dev_id = storage:call("find_device_id", node_id)
if not dev_id then
    print("[exclusion.remove_node]: device not found with node_id: " .. node_id)
    return
end

self:call("clear_device_dependencies", dev_id)

storage:call("remove_device_mapping", node_id)
storage:call("delete_table", storage:call("get_monitoring_handler_key", node_id))
storage:call("remove_device_checking_id", node_id)

core.remove_device(dev_id)
