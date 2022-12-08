local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_AV_CONTROL_POINT" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_SATELLITE_RECEIVER"
         or specific_type == "SPECIFIC_TYPE_SATELLITE_RECEIVER_V2"
    )
) then
    return {
        name = "Remote Control A/V",
        category = "av"
    }
end
