local self, setting_id, value, descriptor = ...

local zwave = require("zwave")
local core = require("core")

core.set_setting_value(setting_id, value, "pending")
zwave.set_wake_up_interval(descriptor.node_id, value)
