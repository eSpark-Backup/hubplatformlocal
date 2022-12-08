local self, index, slot, splitted_values = ...

local indexes_map, slots_changes = self:call("get_slots_mappings")

if not index then
    print("cannot remove slot " .. slot)
    return
end

assert(index <= #splitted_values)

table.remove(splitted_values, index)
table.remove(indexes_map, index)



