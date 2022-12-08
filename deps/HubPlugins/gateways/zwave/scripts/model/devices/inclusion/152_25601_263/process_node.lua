local self, zwave_node = ...


table.remove(zwave_node.channels, 2) -- 1st channel

for _, channel in ipairs(zwave_node.channels) do
    if channel.id == 2 then
        for _, cc in ipairs(channel.classes) do
            if cc.id == 49 then
                cc.sensors = {{
                    sensor_type = "humidity",
                    scales = { "percent" },
                }}
            end
        end
        break
    end
end

return self:_super_call("default", "process_node", zwave_node)
