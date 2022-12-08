local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestRFSignalSensor",
    category = "level_sensor",
    subcategory = "frequency",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "frequency" })

info.child_devices = {
    device_factory:create_device({ "rf_signal_strength" })
}

return info
