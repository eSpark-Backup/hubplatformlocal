local self, values = ...

local result = { }
local group = 1

for _, v in ipairs(values) do
    self:call("_split_value", v, function(sub_value)
        table.insert(result, {
                value = sub_value,
                group = group
            })
    end)

    group = group + 1
end

return result