local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestTemperatureSensor",
    category = "temperature",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "temp" }
}

return info
