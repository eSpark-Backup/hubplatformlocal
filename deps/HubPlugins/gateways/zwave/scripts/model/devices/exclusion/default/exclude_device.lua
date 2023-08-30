local self, device_id = ...

local core = assert(require("core"), "Fail to load the core functionality")
local zwave = assert(require("zwave"), "Fail to load the zwave functionality")
local storage_helper = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local storage = assert(require("storage"), "Fail to load the storage functionality")


local node_id = storage_helper:call("find_node_id", device_id)
local is_device_mapping_correct = node_id and storage_helper:call("find_device_id", node_id) == device_id
if not node_id or not is_device_mapping_correct then
    print("Fail to find node_id for deviceId = " .. device_id)
    self:call("remove_incomplete_device", device_id, node_id )
    if not is_device_mapping_correct then
        storage.delete(device_id)
    end

elseif not zwave.has_node(node_id) then
    self:call("remove_node", node_id)

    local device = core.get_device(device_id)
    if device then
        core.remove_device(device_id)
    end

else
    zwave.start_exclude(node_id)
end
