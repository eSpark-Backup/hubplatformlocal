local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "Siren Alarm",
    category = "siren",
    subcategory = "",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true,
    items = { "siren_alarm" }
}

return info
