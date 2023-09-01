local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "Heat Sensor",
    category = "security_sensor",
    subcategory = "heat",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "heat_alarm", "security_threat" }
}

return info
