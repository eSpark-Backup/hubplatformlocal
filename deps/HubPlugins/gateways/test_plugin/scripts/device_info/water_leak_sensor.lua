local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestWaterLeakSensor",
    category = "security_sensor",
    subcategory = "leak",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "water_leak_alarm", "security_threat" }
}

return info
