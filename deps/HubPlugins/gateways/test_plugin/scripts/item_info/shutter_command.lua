return {
    name = "shutter_command",
    value_type = "token",
    enum = { 
         "change_roll_direction",
         "off",
         "store",
         "learn_upper",
         "learn_lower",
         "learn_favorite"
    },
    value = "off",
    has_getter = false,
    has_setter = true
}
