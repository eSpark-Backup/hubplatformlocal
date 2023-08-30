local info = {}

info.meter_to_item_name = {
    voltage = "electric_meter_volt",
    current = "electric_meter_amper",
    power = "electric_meter_watt",
    consumed_energy = "electric_meter_kwh"
}

info.meter_to_scale = {
    voltage = "volt",
    current = "ampere",
    power = "watt",
    consumed_energy = "kilo_watt_hour"
}

return info
