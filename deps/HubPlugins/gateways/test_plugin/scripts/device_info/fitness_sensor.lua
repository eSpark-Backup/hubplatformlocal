local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestFitnessSensor",
    category = "level_sensor",
    subcategory = "health",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "weight" })

info.child_devices = {
    device_factory:create_device({ "basal_metabolic_rate" }),
    device_factory:create_device({ "blood_pressure" }),
    device_factory:create_device({ "body_mass_index" }),
    device_factory:create_device({ "bone_mass" }),
    device_factory:create_device({ "fat_mass" }),
    device_factory:create_device({ "heart_rate" }),
    device_factory:create_device({ "heart_rate_lf_hf_ratio" }),
    device_factory:create_device({ "muscle_mass" }),
    device_factory:create_device({ "respiratory_rate" }),
    device_factory:create_device({ "total_body_water" })
}

return info
