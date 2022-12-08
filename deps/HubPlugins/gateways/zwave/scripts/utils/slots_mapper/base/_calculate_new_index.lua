local self, index = ...

local new_index = index
local indexes_map, slots_changes = self:call("get_slots_mappings")

for slot, change in pairs(slots_changes) do
    if change.index and change.index < index then
        new_index = new_index - 1
        assert(new_index > 0)
    end
end

for index_for_removing, slot_info in ipairs(indexes_map) do
    if index_for_removing >= new_index then
        break
    elseif slots_changes[slot_info.slot] then
        new_index = new_index + 1
    end
end

return new_index
