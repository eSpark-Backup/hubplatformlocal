local self, zwave_node_id = ...

print("[daemon.start] node id: " .. zwave_node_id)

local timer = require("timer")

local configs = self:call("_get_timers_config", zwave_node_id)

for _, config in ipairs(configs) do
    print(("[daemon.start] run timer. interval: %d, id: %s, script: %s"):format(config.interval, config.id, config.script))
    timer.set_interval_with_id(config.interval, config.id, config.script, config.script_params)
end
