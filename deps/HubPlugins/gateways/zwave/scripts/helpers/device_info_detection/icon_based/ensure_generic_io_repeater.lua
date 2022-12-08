local icon_type = ...

if icon_type == "ICON_TYPE_GENERIC_REPEATER"
or icon_type == "ICON_TYPE_SPECIFIC_REPEATER_SLAVE"
or icon_type == "ICON_TYPE_SPECIFIC_IR_REPEATER" then
    return {
        name = "Generic I/O Repeater",
        category = "generic_io",
        subcategory = "repeater"
    } 
end
