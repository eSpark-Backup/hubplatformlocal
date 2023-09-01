local self, sub_values = ...

local max_group = 0

local indexes_map = self:call("get_slots_mappings")

for _, slot_info in ipairs(indexes_map) do
    if slot_info.group and slot_info.group > max_group then
        max_group = slot_info.group
    end
end

for _, sub_value in ipairs(sub_values) do
    sub_value.group = sub_value.group + max_group + 1
end

local index = 1
while indexes_map[index] and sub_values[index] do
    if indexes_map[index].group then
        local prev_group = sub_values[index].group
        local new_group = indexes_map[index].group
        
        local function fix_groups(delta)
            local i = index + delta
            while indexes_map[i] and 
                  sub_values[i] and 
                  prev_group == sub_values[i].group and 
                  ( indexes_map[i].group == nil or indexes_map[i].group == new_group ) 
            do
                sub_values[i].group = new_group
                i = i + delta
            end
            return i   
        end

        sub_values[index].group = new_group
        fix_groups(-1)
        index = fix_groups(1)
    else
        index = index + 1
    end
end
