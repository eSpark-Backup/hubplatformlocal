local icon_type = ...

if icon_type == "ICON_TYPE_GENERIC_DISPLAY_SIMPLE"
or icon_type == "ICON_TYPE_GENERIC_SET_TOP_BOX"
or icon_type == "ICON_TYPE_GENERIC_TV" then
    return {
        name = "Generic I/O",
        category = "generic_io",
        subcategory = "generic_io"
    } 
end
