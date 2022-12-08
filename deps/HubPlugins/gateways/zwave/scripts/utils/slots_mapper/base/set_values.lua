local self, new_values, should_process_commands = ...

new_values = new_values or {}

local values = self:call("get_values")

print("slots_mapper:set_values, new values count: " .. #new_values .. " , prev values count: " .. #values)

local prev_indexes_map, prev_slots_changes = self:call("get_slots_mappings")

if next(prev_slots_changes) then
    print("Previous setting session is not finished. New session starts, but it can intersect with previous")
end

local new_splitted_values = self:call("_split_values", new_values)
local splitted_values = self:call("_split_values", values)

assert(#splitted_values == #prev_indexes_map, 
       "It looks core values are broken. If it often happens, need to cache values in storage")

self._slots_changes = {}
local slots_changes = self._slots_changes

self._commands = {}
local commands = should_process_commands and self._commands or {}

local existing_value_indexes = {}
local indexes = {}
local added_value_indexes = {}

for index, _ in ipairs(splitted_values) do
    table.insert(indexes, index)
end

local comparator = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/deep_equal")

--[[ 
Following algorithm has complexity O(#new_splitted_values * #splitted_values).
It can be reimplemented with complexity O(#new_splitted_values * ln(#splitted_values)) or O(#new_splitted_values) but
optimization requires more memory (more code and more temporary tables). Do we need it?  
--]]

local j = 1
for i, new_sub_value in ipairs(new_splitted_values) do
    local found_j
    for _ in ipairs(indexes) do
        j = indexes[j] and j or 1
        if comparator(splitted_values[indexes[j]].value, new_sub_value.value) then
            found_j = j
            break
        end
        j = j + 1
    end

    if found_j then
        table.insert(existing_value_indexes, {
                from = indexes[found_j],
                to = i
            })
        table.remove(indexes, found_j)
    else
        table.insert(added_value_indexes, i)
    end
end

local indexes_map = {}
self._indexes_map = indexes_map
local slots_set = {}

local modified_splitted_values = {}
j = 1
local indexes_for_removing = indexes

for i, sub_value in ipairs(splitted_values) do
    if j <= #indexes_for_removing and indexes_for_removing[j] == i then
        local slot = assert(prev_indexes_map[i]).slot

        if #added_value_indexes > 0 then
            local to_be_modified = new_splitted_values[added_value_indexes[#added_value_indexes]]
            table.insert(commands, { 
                slot = slot, 
                value = to_be_modified.value 
            })

            slots_changes[slot] = { 
                index = added_value_indexes[#added_value_indexes], 
                group = to_be_modified.group 
            }
            table.remove(added_value_indexes, #added_value_indexes)
        else
            table.insert(commands, { slot = slot })

            slots_changes[slot] = {}
        end

        slots_set[slot] = true
        table.insert(indexes_map, { slot = slot })
        table.insert(modified_splitted_values, sub_value)
        j = j + 1
    else
        local existing_value_index = existing_value_indexes[i - j + 1]
        local slot = assert(prev_indexes_map[existing_value_index.from]).slot

        table.insert(indexes_map, { 
            slot = slot,
            group = new_splitted_values[ existing_value_index.to ].group
        })

        slots_set[slot] = true
        table.insert(modified_splitted_values, splitted_values[existing_value_index.from])  
    end
end

self:call("_fix_groups_in_sub_values", modified_splitted_values)
values = self:call("_merge_values", modified_splitted_values)

self._values = values

for _, index_for_adding in ipairs(added_value_indexes) do
    local slot = self:call("_get_free_slot", slots_set)
    slots_set[slot] = true

    slots_changes[slot] = { 
        index = index_for_adding, 
        group = new_splitted_values[index_for_adding].group 
    }

    table.insert(commands, { 
        slot = slot, 
        value = new_splitted_values[index_for_adding].value 
    })
end

self._current_slot = should_process_commands and commands[1] and commands[1].slot

self:call("_save_commands")
self:call("_save_slots_mappings")
self:call("_save_values")

if self._current_slot then
    self:call("_run_command", self._current_slot, commands[1].value)
end

return commands
