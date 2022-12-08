local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_APPLIANCE" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_GENERAL_APPLIANCE"
         or specific_type == "SPECIFIC_TYPE_KITCHEN_APPLIANCE"
         or specific_type == "SPECIFIC_TYPE_LAUNDRY_APPLIANCE"
    )
) then
    return {
        name = "Appliance",
        category = "appliance"
    }
end
