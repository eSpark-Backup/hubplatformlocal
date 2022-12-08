local icon_type = ...

if icon_type == "ICON_TYPE_GENERIC_WINDOW_COVERING_NO_POSITION_ENDPOINT"
or icon_type == "ICON_TYPE_GENERIC_WINDOW_COVERING_ENDPOINT_AWARE"
or icon_type == "ICON_TYPE_GENERIC_WINDOW_COVERING_POSITION_ENDPOINT_AWARE" then
    return {
        name = "Window Covering",
        category = "window_cov",
        subcategory = "window_cov"
    } 
end
