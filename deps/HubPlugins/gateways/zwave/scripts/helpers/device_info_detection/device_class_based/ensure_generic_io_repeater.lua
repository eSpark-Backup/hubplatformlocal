local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_REPEATER_SLAVE" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_REPEATER_SLAVE"
         or specific_type == "SPECIFIC_TYPE_VIRTUAL_NODE"
    )
)
or (generic_type == "GENERIC_TYPE_NETWORK_EXTENDER" 
    and (  specific_type == "SPECIFIC_TYPE_NOT_USED"
        or specific_type == "SPECIFIC_TYPE_SECURE_EXTENDER"
    )
) then
    return {
        name = "Generic I/O Repeater",
        category = "generic_io",
        subcategory = "repeater"
    }
end
