local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestMotionSensor",
    category = "security_sensor",
    subcategory = "motion",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "motion" }
}

info.child_devices = {
    {
        name = "TestTemperatureSensor",
        category = "temperature",
        type = "sensor",
        device_type_id = "test",
        battery_powered = true,
        items = { "temp" }
    },
    {
        name = "TestHumiditySensor",
        category = "humidity",
        type = "sensor",
        device_type_id = "test",
        battery_powered = true,
        items = { "humidity" }
    }
}

return info
