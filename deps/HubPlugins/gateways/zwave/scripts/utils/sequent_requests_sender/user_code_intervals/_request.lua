local self, key, value --[[actually value is nil]] = ...

local slots_mapper = self:call("get_slots_mapper", key)

local commands = slots_mapper:call("set_values", value, true)

if commands == nil or commands[1] == nil then
    self._timer.reset(0, self._timer_id)
end
