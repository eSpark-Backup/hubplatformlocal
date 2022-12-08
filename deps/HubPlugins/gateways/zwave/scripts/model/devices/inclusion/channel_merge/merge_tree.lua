local self, inclusion_context = ...

if self:call("need_merge_channels", inclusion_context) then
    self:call("merge_channels", inclusion_context)
    self:call("simplify_tree", inclusion_context)
else
    self:_super_call("default", "merge_tree", inclusion_context)
end
