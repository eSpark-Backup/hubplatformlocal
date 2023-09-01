local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestSeismicitySensor",
    category = "level_sensor",
    subcategory = "seismicity",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "seismic_intensity" })

info.child_devices = {
    device_factory:create_device({ "seismic_magnitude" }),
    device_factory:create_device({ "tide_level" })
}

return info
