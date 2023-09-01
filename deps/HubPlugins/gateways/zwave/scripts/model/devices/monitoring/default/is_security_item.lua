local self, item = ...

assert(self._device_info)

local security_items = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/security_items")()
local item_names = require("HUB:zwave/scripts/resources/item_names")

if security_items[item.name] then return true end

if self._device_info.panic_triggers then
    for _, trigger in ipairs(self._device_info.panic_triggers) do
        if trigger.item_name and item.name == trigger.item_name then
            return true
        elseif trigger.button_number 
           and item.name == item_names.button_state 
           and item.value.button_number == trigger.button_number then
            return true
        end
    end
end
