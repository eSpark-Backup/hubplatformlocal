local self = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local table_name = self:call("get_slots_mappings_table_name")

local mappings = self._dictionary_id and storage:call("get_table_value", table_name, self._dictionary_id) or 
                 not self._dictionary_id and storage:call("get_table", table_name)

assert(self._indexes_map == nil)
assert(self._slots_changes == nil)

local function make_number_key_map(loaded_map)
    if not loaded_map then
        return {}
    end

    local should_fix_keys
    for index in pairs(loaded_map) do
        if type(index) ~= "number" then
            should_fix_keys = true
            break
        end
    end

    if not should_fix_keys then
        return loaded_map
    end

    local result = {}
    for index, key in pairs(loaded_map or {}) do
        result[tonumber(index)] = key
    end
    return result
end

self._indexes_map = make_number_key_map(mappings and mappings.indexes_map)
self._slots_changes = make_number_key_map(mappings and mappings.slots_changes)
self._current_slot = mappings and mappings.current_slot

return self._indexes_map, self._slots_changes, self._current_slot
