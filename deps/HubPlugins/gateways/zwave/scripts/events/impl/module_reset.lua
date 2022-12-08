local params = ...

local status = params.status

print("Reset module: " .. tostring(status))

local core = assert(require("core"), "Fail to load the 'core' functionality")

if status == nil or status == "finished" then
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/clear_storages")()

    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/update_gateway_roles")("wizard", "primary")
end

if status ~= nil then
    core.send_ui_broadcast({ event = params.event, status = status })
end
