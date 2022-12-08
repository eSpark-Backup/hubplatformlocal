local self, info = ...

if info.port then
    self:call("set_setting_value_to_core", "zwave.port", info.port)
end
