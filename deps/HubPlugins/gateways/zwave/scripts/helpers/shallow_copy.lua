local orig = ...

local copy
if type(orig) == 'table' then
    copy = {}
    for orig_key, orig_value in pairs(orig) do
        copy[orig_key] = orig_value
    end
else -- number, string, boolean, etc
    copy = orig
end

return copy

