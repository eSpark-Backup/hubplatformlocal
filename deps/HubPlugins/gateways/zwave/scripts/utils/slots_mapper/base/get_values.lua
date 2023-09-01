local self = ...

if self._values == nil then
    self._values = self:call("_load_values")
    assert(self._values ~= nil)
end

return self._values