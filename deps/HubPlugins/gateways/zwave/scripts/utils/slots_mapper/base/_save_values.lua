local self = ...

if self._values == nil then
    return
end

assert(self._item_id)
local core = require "core"

if self._dictionary_id == nil then
    core.update_item_value(self._item_id, self._values)
elseif #self._values > 0 then
        core.update_item_dictionary_value(self._item_id, self._dictionary_id, self._values)
else
    core.remove_item_dictionary_value(self._item_id, self._dictionary_id)
end
