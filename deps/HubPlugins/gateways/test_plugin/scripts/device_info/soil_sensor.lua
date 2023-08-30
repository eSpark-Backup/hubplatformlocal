local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestSoilSensor",
    category = "level_sensor",
    subcategory = "soil",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "soil_temperature" })

info.child_devices = {
    device_factory:create_device({ "soil_humidity" }),
    device_factory:create_device({ "soil_reactivity" }),
    device_factory:create_device({ "soil_salinity" })
}

return info
