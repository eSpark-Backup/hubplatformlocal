local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestGarageDoor",
    category = "garage_door",
    type = "shutter.garage",
    device_type_id = "test",
    battery_powered = false,
    items = { "barrier", "barrier_problem_sensors" }
}

return info
