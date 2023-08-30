local self = ...

assert(self._item_id)
local core = require "core"

if self._dictionary_id == nil then
    local item = core.get_item(self._item_id)
    assert(item, "cannot find item " .. self._item_id)

    self._values = item.value or {}
else
    self._values = core.get_item_dictionary_value(self._item_id, self._dictionary_id) or {} 
end

return self._values
