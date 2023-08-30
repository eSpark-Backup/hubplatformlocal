local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestWeatherSensor",
    category = "weather",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "atmospheric_pressure" })

info.child_devices = {
    device_factory:create_device({ "barometric_pressure" }),
    device_factory:create_device({ "humidity" }),
    device_factory:create_device({ "outside_temperature" }),
    device_factory:create_device({ "rain_rate" }),
    device_factory:create_device({ "solar_radiation" }),
}

return info
