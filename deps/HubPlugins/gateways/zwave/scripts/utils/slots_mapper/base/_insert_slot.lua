local self, slot, value, splitted_values = ...

assert(value ~= nil)

local indexes_map, slots_changes = self:call("get_slots_mappings")

if slots_changes[slot] then
    local new_index = self:call("_calculate_new_index", slots_changes[slot].index)

    assert(new_index and new_index <= #splitted_values + 1, "invalid new index " .. new_index)
    
    table.insert(splitted_values, new_index, { 
        value = value, 
        group = slots_changes[slot].group 
    })

    table.insert(indexes_map, new_index, { 
        slot = slot, 
        group = slots_changes[slot].group 
    })

else
    self:call("_append_unknown_slot_value", splitted_values, indexes_map, slot, value)
end
