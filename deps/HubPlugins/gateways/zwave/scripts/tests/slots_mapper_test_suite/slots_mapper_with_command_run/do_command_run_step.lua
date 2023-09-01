local self = ...

local storage = require "storage"

local table_name = "saved_command." .. self._item_id .. "." .. tostring(self._dictionary_id)

local command = storage.get_table(table_name)

if command then
    storage.delete(table_name)
    self:call("on_slot_value_changed", command.slot, command.value)
end

return command