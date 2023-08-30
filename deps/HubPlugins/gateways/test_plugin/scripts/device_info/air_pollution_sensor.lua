local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestAirPollutionSensor",
    category = "level_sensor",
    subcategory = "particulate_matter",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "ammonia" })

info.child_devices = {
    device_factory:create_device({ "co2_level" }),
    device_factory:create_device({ "co_level" }),
    device_factory:create_device({ "formaldehyde_level" }),
    device_factory:create_device({ "lead" }),
    device_factory:create_device({ "methane_density" }),
    device_factory:create_device({ "nitrogen_dioxide" }),
    device_factory:create_device({ "ozone" }),
    device_factory:create_device({ "particulate_matter_1" }),
    device_factory:create_device({ "particulate_matter_10" }),
    device_factory:create_device({ "particulate_matter_2_dot_5" }),
    device_factory:create_device({ "radon_concentration" }),
    device_factory:create_device({ "smoke_density" }),
    device_factory:create_device({ "sulfur_dioxide" }),
    device_factory:create_device({ "volatile_organic_compound_level" })
}

return info
