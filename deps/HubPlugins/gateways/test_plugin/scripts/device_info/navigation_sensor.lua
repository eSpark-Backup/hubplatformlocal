local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestNavigationSensor",
    category = "level_sensor",
    subcategory = "navigation",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "acceleration_x_axis", "acceleration_y_axis", "acceleration_z_axis" })

info.child_devices = {
    device_factory:create_device({ "angle_position" }),
    device_factory:create_device({ "direction" }),
    device_factory:create_device({ "distance" }),
    device_factory:create_device({ "motion_direction" }),
    device_factory:create_device({ "velocity" })
}

return info
