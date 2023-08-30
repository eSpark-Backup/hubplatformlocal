local params = ...

require ("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local node_id = storage:call("find_node_id", params.device_id)
assert(node_id, "cannot find node")

for _, firmware in ipairs(params.firmwares) do
    print("Download node firmware: node " .. node_id .. ", " .. " target " .. firmware.target .. ", path " .. firmware.path)
    zwave.download_node_firmware(node_id, firmware.target, firmware.path)
end
