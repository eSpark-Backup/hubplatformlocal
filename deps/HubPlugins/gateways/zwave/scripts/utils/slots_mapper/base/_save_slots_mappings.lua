local self = ...

if self._indexes_map == nil and self._slots_changes == nil then
    return
end

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local indexes_map, slots_changes, current_slot = self:call("get_slots_mappings")

local value = { 
    indexes_map = indexes_map,
    slots_changes = slots_changes,
    current_slot = current_slot
}

local table_name = self:call("get_slots_mappings_table_name")

if self._dictionary_id then
    storage:call("set_table_value", table_name, self._dictionary_id, value)
else
    storage:call("set_table", table_name, value)
end
