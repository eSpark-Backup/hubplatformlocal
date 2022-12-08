local self = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local table_name = self:call("get_commands_table_name")

local commands = self._dictionary_id and storage:call("get_table_value", table_name, self._dictionary_id) or 
                 not self._dictionary_id and storage:call("get_table", table_name)

assert(self._commands == nil)
self._commands = commands or {}

return self._commands