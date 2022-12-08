local self, zwave_node_id = ...

print("[daemon.stop] node id: " .. zwave_node_id)

local timer = require("timer")

local configs = self:call("_get_timers_config", zwave_node_id)

for _, config in ipairs(configs) do
    print(("[daemon.stop] cancel timer. id: %s"):format(config.id))
    timer.cancel(config.id)
end
