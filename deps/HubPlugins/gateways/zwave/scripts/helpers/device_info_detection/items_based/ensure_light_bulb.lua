local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.dimmer] ~= nil
and not hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/has_electric_meter")(items) then
    return {
        name = "Light",
        category = "dimmable_light",
        subcategory = "dimmable_bulb"
    }
end
