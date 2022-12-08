local self, splitted_values, indexes_map, cb = ...

local function it(splitted_values, i)
    if splitted_values[i] and indexes_map[i].unknown then
        return self:call("_handle_frame", splitted_values, i, -1, cb ), i
    end
end

return it, splitted_values, #splitted_values
