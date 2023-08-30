local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestStopwatchSensor",
    category = "level_sensor",
    subcategory = "time",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "time_period" }
}

return info
