local items = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if items[item_names.dimmer] ~= nil
and hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/device_info_detection/items_based/has_electric_meter")(items) then
    return {
        name = "Dimmable Plug",
        category = "dimmable_light",
        subcategory = "dimmable_plugged"
    }
end
