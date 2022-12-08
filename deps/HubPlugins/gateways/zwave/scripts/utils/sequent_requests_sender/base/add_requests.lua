local self, ids = ...

if #ids > 0 then
    self:call("remove_postponed_requests", ids)

    self:call("_add_requests_impl", ids)
end
