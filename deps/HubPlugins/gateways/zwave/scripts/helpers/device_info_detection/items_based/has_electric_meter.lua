local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

return items[item_names.electric_meter_kwh] ~= nil
    or items[item_names.electric_meter_kvah] ~= nil
    or items[item_names.electric_meter_watt] ~= nil
    or items[item_names.electric_meter_pulse] ~= nil
    or items[item_names.electric_meter_volt] ~= nil
    or items[item_names.electric_meter_amper] ~= nil
    or items[item_names.electric_meter_factor] ~= nil
    or items[item_names.electric_meter_kvar] ~= nil
    or items[item_names.electric_meter_kvarh] ~= nil
