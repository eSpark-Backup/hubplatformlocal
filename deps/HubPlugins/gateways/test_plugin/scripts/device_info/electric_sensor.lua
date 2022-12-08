local info = loadfile("HUB:test_plugin/scripts/device_info/default")()

local device_factory = loadfile("HUB:test_plugin/scripts/utils/device_factory")({
    name = "TestElectricSensor",
    category = "level_sensor",
    subcategory = "electricity",
    type = "sensor",
    device_type_id = "test",
    battery_powered = true
})

info.master_device = device_factory:create_device({ "electric_meter_amper", "electric_meter_watt", "electric_meter_volt" })

info.child_devices = {
    device_factory:create_device({ "electrical_conductivity" }),
    device_factory:create_device({ "electrical_resistance" })
}

return info
