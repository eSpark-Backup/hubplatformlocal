return {
    name = "shutter_state",
    value_type = "token",
    enum = { 
         "idle",
         "learn_upper",
         "change_roll_direction",
         "learn_lower",
         "learn_favorite"
    },
    value = "idle",
    has_getter = true,
    has_setter = false
}
