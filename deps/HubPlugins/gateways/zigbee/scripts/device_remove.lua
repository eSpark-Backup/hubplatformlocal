local params = ...

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local device_id = params.deviceId

print("LUA: Try remove device: " .. device_id)
assert( device_id, "Device remove deviceId: nodeId required" )

local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local node_id = storage:call( "find_node_id", device_id )

local core = assert(require("core"), "Fail to load the 'core' functionality")
if core.notify_process_started then
    core.notify_process_started("removing_device")
end

if node_id then
    local zigbee = assert( require "zigbee", "Fail to load the zigbee functionality" )
    zigbee.remove_node(node_id)
else
    local device = core.get_device(device_id)
    if device then
        local device_type_id = assert(device.device_type_id)
        local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_exclusion")(nil, nil, device_type_id)
        dev_handler:remove_device(device_id)

        if core.notify_process_stopped then
            core.notify_process_stopped("removing_device", "success")
        end
    end
end
