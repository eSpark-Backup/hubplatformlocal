local params = ...

local node_id = params.node_id
local status = params.status

print("Device check status: " .. tostring(status))

local core = assert(require("core"), "Fail to load the 'core' functionality")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

if status == "timeout" then status = "failed" end

local device_id = storage:call("find_device_checking_id", node_id)

if device_id then
    if status == "finished" or status == "failed" then
        storage:call("remove_device_checking_id", node_id)
    end
    
    core.send_device_checked_broadcast({device_id = device_id, status = status})
else
    print("Unable to find deviche checking id by node id: " .. node_id)
end
