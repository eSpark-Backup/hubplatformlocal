local self, zwave_node = ...

-- this device supports 2 additional channels:
-- 1 is used to Set/Get the state of output load
-- 2 is used to Set/Get the brightness level of RGB
-- second channel cannot be used without changing device configuration( 0x51 (81) )
-- until we support custom device configuration we have disabled that channels
-- all functionality are available via Root(0) channel

if #zwave_node.channels == 3 then
    table.remove(zwave_node.channels)
    table.remove(zwave_node.channels)
end

return self:_super_call("default", "process_node", zwave_node)