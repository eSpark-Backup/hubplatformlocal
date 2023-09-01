local item_names = require("HUB:zwave/scripts/resources/item_names")

return  {
        kilo_watt_hour = {
            item_name = item_names.electric_meter_kwh,
            ui_scale = "kilo_watt_hour"
        },
        kilo_volt_ampere_hour = {
            item_name = item_names.electric_meter_kvah,
            ui_scale = "kilo_volt_ampere_hour"
        },
        watt = {
            item_name = item_names.electric_meter_watt,
            ui_scale = "watt"
        },
        pulse_count = {
            item_name = item_names.electric_meter_pulse,
        },
        volt = {
            item_name = item_names.electric_meter_volt,
            ui_scale = "volt"
        },
        ampere = {
            item_name = item_names.electric_meter_amper,
            ui_scale = "ampere"
        },
        power_factor = {
            item_name = item_names.electric_meter_factor,
            ui_scale = "float"
        },
        kilo_volt_ampere_reactive = {
            item_name = item_names.electric_meter_kvar,
            ui_scale = "kilo_volt_amper_reactive"
        },
        kilo_volt_ampere_reactive_hour = {
            item_name = item_names.electric_meter_kvarh,
            ui_scale = "kilo_volt_amper_reactive_hour"
        }
    }