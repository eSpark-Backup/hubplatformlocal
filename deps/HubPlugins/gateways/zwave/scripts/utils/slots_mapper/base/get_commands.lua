local self = ...

if self._commands == nil then
    self:call("_load_commands")
    assert(self._commands)
end

return self._commands