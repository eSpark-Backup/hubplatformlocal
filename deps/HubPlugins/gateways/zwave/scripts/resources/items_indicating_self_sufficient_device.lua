local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    [item_names.electric_meter_kwh] = true,
    [item_names.electric_meter_kvah] = true,
    [item_names.electric_meter_watt] = true,
    [item_names.electric_meter_pulse] = true,
    [item_names.electric_meter_volt] = true,
    [item_names.electric_meter_amper] = true,
    [item_names.electric_meter_factor] = true,
    [item_names.electric_meter_kvar] = true,
    [item_names.electric_meter_kvarh] = true,
    [item_names.switch] = true,
    [item_names.rgb_color] = true,
    [item_names.dimmer] = true,
    [item_names.dimmer_up] = true,
    [item_names.dimmer_down] = true,
    [item_names.dimmer_stop] = true,
    [item_names.button_state] = true,
    [item_names.barrier] = true,
}
