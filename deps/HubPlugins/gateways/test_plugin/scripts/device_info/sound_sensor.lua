local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestSoundSensor",
    category = "level_sensor",
    subcategory = "sound",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "loudness" }
}

return info
