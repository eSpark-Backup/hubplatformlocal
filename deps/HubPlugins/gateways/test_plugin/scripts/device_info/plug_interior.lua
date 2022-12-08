local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestSwitch",
    category = "switch",
    subcategory = "interior_plugin",
    type = "switch.outlet",
    device_type_id = "test",
    battery_powered = false,
    items = { "switch" }
}

return info
