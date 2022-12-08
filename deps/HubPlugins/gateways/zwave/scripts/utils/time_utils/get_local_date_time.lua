local self = ...

local local_date_time = self._core.get_local_time()

return self:call("parse_date_time", local_date_time)
