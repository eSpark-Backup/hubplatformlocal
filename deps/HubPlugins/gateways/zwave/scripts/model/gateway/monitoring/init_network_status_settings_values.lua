local self = ...

local zwave = require("zwave")
local info = zwave.get_info()

self:call("update_network_status_setting/zwave.home_id", info)
self:call("update_network_status_setting/zwave.locale", info)
self:call("update_network_status_setting/zwave.node_id", info)
self:call("update_network_status_setting/zwave.port", info)
self:call("update_network_status_setting/zwave.role", info)
self:call("update_network_status_setting/zwave.version", info)
