local self = ...

if self._commands == nil then
    return
end

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local table_name = self:call("get_commands_table_name")

if self._dictionary_id then
    storage:call("set_table_value", table_name, self._dictionary_id, self._commands)
else
    storage:call("set_table", table_name, self._commands)
end