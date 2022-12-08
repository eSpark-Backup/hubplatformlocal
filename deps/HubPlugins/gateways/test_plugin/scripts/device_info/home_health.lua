local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "Home Health",
    category = "generic_sensor",
    subcategory = "",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "position" }
}

return info
