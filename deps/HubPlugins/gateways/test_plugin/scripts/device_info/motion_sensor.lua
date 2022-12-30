local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestMotionSensor",
    category = "security_sensor",
    subcategory = "motion",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "motion", "security_threat" }
}

return info
