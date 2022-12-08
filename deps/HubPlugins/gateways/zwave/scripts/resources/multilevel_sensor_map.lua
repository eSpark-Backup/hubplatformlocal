local item_names = require("HUB:zwave/scripts/resources/item_names")

local sensor_type_to_item_name = {
    air_temperature = item_names.temp,
    illuminance = item_names.lux,
    time = item_names.time_period,
    power = item_names.electric_meter_watt,
    voltage = item_names.electric_meter_volt,
    current = item_names.electric_meter_amper,
    electrical_resistivity = item_names.electrical_resistance,
    general_purpose = item_names.relative_modulation_level,
}

local sensor_type_to_item_name_method = function(sensor_type)
    return sensor_type_to_item_name[sensor_type] or sensor_type
end


return {
    sensor_type_to_item_name_method = sensor_type_to_item_name_method,
}
