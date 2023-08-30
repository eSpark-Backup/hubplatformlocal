local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestDimmer",
    category = "dimmable_light",
    subcategory = "dimmable_plugged",
    type = "dimmer.outlet",
    device_type_id = "test",
    battery_powered = false,
    items = { "dimmer", "dimmer_up", "dimmer_down", "dimmer_stop" }
}

return info
