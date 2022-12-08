local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestLightSensor",
    category = "light_sensor",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "lux" }
}

return info
