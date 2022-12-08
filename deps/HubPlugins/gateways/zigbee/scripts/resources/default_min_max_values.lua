return {
    { cluster_name = "temperature_measurement", item_name = "temp", min = {value = 0, scale = "celsius"}, max = {value = 40, scale = "celsius"} },
    { cluster_name = "relative_humidity", item_name = "humidity", min = {value = 0, scale = "percent"}, max = {value = 100, scale = "percent"} },
    { cluster_name = "thermostat", item_name = "thermostat_setpoint_cooling", min = {value = 0, scale = "celsius"}, max = {value = 40, scale = "celsius"} },
    { cluster_name = "thermostat", item_name = "thermostat_setpoint_heating", min = {value = 0, scale = "celsius"}, max = {value = 40, scale = "celsius"} },
    { cluster_name = "illuminance_measurement", item_name = "lux", min = {value = 1, scale = "lux"}, max = {value = 3576000, scale = "lux"} },
}
