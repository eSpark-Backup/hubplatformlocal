local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestWaterSensor",
    category = "level_sensor",
    subcategory = "water",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "domestic_hot_water_temperature" })

info.child_devices = {
    device_factory:create_device({ "relative_modulation_level" }),
    device_factory:create_device({ "tank_capacity" }),
    device_factory:create_device({ "target_temperature" }),
    device_factory:create_device({ "water_acidity" }),
    device_factory:create_device({ "water_chlorine_level" }),
    device_factory:create_device({ "water_flow" }),
    device_factory:create_device({ "water_oxidation_reduction_potential" }),
    device_factory:create_device({ "water_pressure" }),
    device_factory:create_device({ "water_temperature" })
}

return info
