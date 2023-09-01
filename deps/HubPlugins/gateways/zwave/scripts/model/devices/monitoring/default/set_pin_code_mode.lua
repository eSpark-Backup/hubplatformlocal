local self, item_id, descriptor, pin_code_id, mode = ...

local core = require("core")

local item = assert(core.get_item(item_id))

local pin_code_value = assert(item.value[pin_code_id])
pin_code_value.mode = pin_code_value.mode or "enabled"

if pin_code_value.mode ~= mode then
    pin_code_value.mode = mode
    self:call("set_item_dictionary_value", item_id, descriptor, pin_code_id, "userCode", pin_code_value)
end
