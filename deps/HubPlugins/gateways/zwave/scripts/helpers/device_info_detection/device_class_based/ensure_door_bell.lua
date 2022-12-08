local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_AV_CONTROL_POINT" 
    and (   specific_type == "SPECIFIC_TYPE_DOORBELL"
         or specific_type == "SPECIFIC_TYPE_SOUND_SWITCH"
    )
) then
    return {
        name = "Door Bell",
        category = "doorbell"
    }
end
