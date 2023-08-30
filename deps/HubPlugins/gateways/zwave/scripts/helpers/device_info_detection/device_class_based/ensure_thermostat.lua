local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_THERMOSTAT" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_THERMOSTAT_GENERAL"
         or specific_type == "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2"
    )
) then
    return {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac"
    }
end
