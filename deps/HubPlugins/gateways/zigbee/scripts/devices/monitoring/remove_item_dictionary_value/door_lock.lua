local handler, item_id, descriptor, key = ...

local zigbee = require("zigbee")
local core = require("core")

local item = core.get_item(item_id)

assert(key)
zigbee.send_command( 
    descriptor.node_id,
    descriptor.endpoint_id, 
    descriptor.cluster_name, 
    "ClearPinCode",
    tonumber(key)
)
