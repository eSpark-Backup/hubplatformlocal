local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestAirConditionerSensor",
    category = "level_sensor",
    subcategory = "modulation",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "boiler_water_temperature" })

info.child_devices = {
    device_factory:create_device({ "condenser_coil_temperature" }),
    device_factory:create_device({ "defrost_temperature" }),
    device_factory:create_device({ "dew_point" }),
    device_factory:create_device({ "discharge_line_temperature" }),
    device_factory:create_device({ "discharge_pressure" }),
    device_factory:create_device({ "evaporator_coil_temperature" }),
    device_factory:create_device({ "exhaust_temperature" }),
    device_factory:create_device({ "liquid_line_temperature" }),
    device_factory:create_device({ "moisture" }),
    device_factory:create_device({ "return_air_temperature" }),
    device_factory:create_device({ "suction_pressure" }),
    device_factory:create_device({ "supply_air_temperature" })
}

return info
