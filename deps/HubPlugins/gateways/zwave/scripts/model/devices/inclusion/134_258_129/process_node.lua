local self, zwave_node = ...

-- this device has several endpoints without functionality

for _, channel in ipairs(zwave_node.channels) do
    if channel.id == 0 then
        zwave_node.channels = {channel}
        break
    end
end

return self:_super_call("default", "process_node", zwave_node)
