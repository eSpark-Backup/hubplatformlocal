local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_THERMOSTAT" 
    and (specific_type == "SPECIFIC_TYPE_THERMOSTAT_HEATING"
    )
) then
    return {
        name = "Thermostat",
        category = "hvac",
        subcategory = "heater"
    }
end
