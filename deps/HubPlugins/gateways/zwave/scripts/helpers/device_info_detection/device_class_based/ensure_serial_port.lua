local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_SWITCH_BINARY" 
    and (specific_type == "SPECIFIC_TYPE_POWER_STRIP"
    )
) then
    return {
        name = "Serial Port",
        category = "serial_port"
    }
end
