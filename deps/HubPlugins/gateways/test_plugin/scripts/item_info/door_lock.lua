return { 
    name = "door_lock",
    value_type = "token",
    has_getter = true,
    has_setter = true,
    enum = {
        "unsecured",
        "unsecured_with_timeout",
        "unsecured_for_inside",
        "unsecured_for_inside_with_timeout",
        "unsecured_for_outside",
        "unsecured_for_outside_with_timeout",
        "unknown",
        "secured"
    },
    value = "unknown"
}
