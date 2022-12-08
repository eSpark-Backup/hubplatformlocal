local self, ids = ...

self:call("_remove_requests_impl", ids)

self:call("remove_postponed_requests", ids)
