local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestHeatingMeter",
    category = "power_meter",
    type = "meter.power",
    device_type_id = "test",
    battery_powered = true,
    items = { "heating_meter_energy", "meter_reset" }
}

return info
