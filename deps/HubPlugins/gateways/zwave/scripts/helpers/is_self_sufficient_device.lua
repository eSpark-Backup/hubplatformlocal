local device_data = ...

local items_indicating_self_sufficient_device = hubplugins.weak_require("HUB:zwave/scripts/resources/items_indicating_self_sufficient_device")

for _, item in ipairs(device_data.items) do
    if items_indicating_self_sufficient_device[item.name] then
        return true
    end
end

return false
