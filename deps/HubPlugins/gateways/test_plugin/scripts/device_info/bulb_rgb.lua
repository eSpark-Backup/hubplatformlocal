local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestBulbRgb",
    category = "dimmable_light",
    subcategory = "dimmable_colored",
    type = "light.bulb",
    device_type_id = "test",
    battery_powered = false,
    items = { "dimmer", "dimmer_up", "dimmer_down", "dimmer_stop", "rgbcolor" }
}

return info
