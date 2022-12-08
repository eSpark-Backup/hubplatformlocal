local self, node_id, progress, status, error = ...

local core = assert(require("core"), "Fail to load the 'core' functionality")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local device_id = storage:call("find_device_id", node_id)
assert(device_id, "Cannot find device id for node " .. node_id)

core.send_ui_broadcast({
    event = "rediscover_device_progress",
    progress = progress,
    status = status,
    device_id = device_id,
    error = error
})

local status_to_setting_value = { started = "pending", finished = "synced", failed = "failed" }
local status_to_device_status = { started = "rediscovering", finished = "idle", failed = "idle" }

local setting = self:call("find_setting", node_id, nil, "rediscover")

if setting and status_to_setting_value[status] then
    core.update_setting_status(setting.id, status_to_setting_value[status])
end

if status_to_device_status[status] then
    core.update_device_status(device_id, status_to_device_status[status])
end

if status == "finished" then
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/run_node_rediscover_migration")(node_id)
end
