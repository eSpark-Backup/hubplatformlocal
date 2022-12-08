local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestElectricMeter",
    category = "power_meter",
    type = "meter.power",
    device_type_id = "test",
    battery_powered = true,
    items = {
        "electric_meter_amper",
        "electric_meter_power_factor",
        "electric_meter_kvah",
        "electric_meter_kvar",
        "electric_meter_kvarh",
        "electric_meter_kwh",
        "electric_meter_pulse",
        "electric_meter_volt",
        "electric_meter_watt",
        "meter_reset"
    }
}

return info
