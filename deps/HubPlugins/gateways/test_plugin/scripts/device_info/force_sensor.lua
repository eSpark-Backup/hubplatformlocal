local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestForceSensor",
    category = "generic_sensor",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "applied_force_on_sensor" }
}

return info
