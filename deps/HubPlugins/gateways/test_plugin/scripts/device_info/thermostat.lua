local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestThermostat",
    category = "hvac",
    subcategory = "hvac",
    type = "thermostat",
    device_type_id = "test",
    battery_powered = true,
    items = { 
        "thermostat_mode", 
        "thermostat_operating_state", 
        "thermostat_fan_mode",
        "thermostat_fan_state",
        "thermostat_setpoint",
        "thermostat_setpoint_heating",
        "thermostat_setpoint_cooling",
        "thermostat_energy_saving_mode"
    }
}

return info
