local params = ...

print("Reset module")

local core = assert(require("core"), "Fail to load the 'core' functionality")

local status = params.status
if status == nil or status == "finished" then
    hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/clear_storages")()
end

if status ~= nil then
    core.send_ui_broadcast({ event = params.event, status = status })
end
