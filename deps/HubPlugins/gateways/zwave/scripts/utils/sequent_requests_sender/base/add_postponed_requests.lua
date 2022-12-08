local self, ids = ...

if #ids > 0 then
    self:call("_remove_requests_impl", ids)

    self:call("_add_postponed_requests_impl", ids)
end
