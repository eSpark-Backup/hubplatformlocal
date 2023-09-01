local params = ...

require ("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = require "zwave"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local node_id = storage:call("find_node_id", params.device_id)
assert(node_id, "Node not found for device " .. params.device_id)

zwave.start_rediscover(node_id, "HUB:zwave/configs/device_setting.json")