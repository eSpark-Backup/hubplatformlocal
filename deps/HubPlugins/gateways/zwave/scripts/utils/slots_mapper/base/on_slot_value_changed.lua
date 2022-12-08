local self, slot, value = ...

print("slots_mapper:on_slot_value_changed, slot: " .. slot .. " value: " .. hubplugins.dump(value))

local values = self:call("get_values")

local splitted_values = self:call("_split_values", values)
self:call("_fix_groups_in_sub_values", splitted_values)

local indexes_map, slots_changes, current_slot = self:call("get_slots_mappings")

local slot_index
for i, slot_info in ipairs(indexes_map) do
    if slot_info.slot == slot then
        slot_index = i
        break
    end
end

local new_slot_change

if value == nil then
    self:call("_erase_slot", slot_index, slot, splitted_values)

    if slots_changes[slot] and slots_changes[slot].index then
        new_slot_change = slots_changes[slot]
        new_slot_change.removed = true -- investigate about using of it
    end
elseif slot_index then
    if slots_changes[slot] == nil or slots_changes[slot].index == nil then
        if not splitted_values[slot_index] then
            splitted_values[slot_index] = {}
        end

        splitted_values[slot_index].value = value
    else
        self:call("_erase_slot", slot_index, slot, splitted_values)
        self:call("_insert_slot", slot, value, splitted_values)
    end
else
    self:call("_insert_slot", slot, value, splitted_values)
end

slots_changes[slot] = new_slot_change
self._values = self:call("_merge_values", splitted_values)

local next_command

if slot == current_slot then
    local commands = self:call("get_commands")
    for index, command in ipairs(commands) do
        if command.slot == slot then
            next_command = commands[index + 1]
            break
        end
    end
    self._current_slot = next_command and next_command.slot

    if not next_command then
        self._commands = {}
        self._slots_changes = {}
        self:call("_save_commands")
        print("Values setting session has been finished")
    end
end

self:call("_save_slots_mappings")
self:call("_save_values")

if next_command then
    self:call("_run_command", next_command.slot, next_command.value)
end

return next_command

