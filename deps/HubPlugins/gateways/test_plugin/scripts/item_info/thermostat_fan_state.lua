return {
    name = "thermostat_fan_state",
    value_type = "token",
    has_getter = true,
    has_setter = false,
    enum = {
        "idle_off",
        "running_low",
        "running_high",
        "running_medium",
        "circulation_mode",
        "humidity_circulation_mode",
        "right_left_circulation_mode",
        "up_down_circulation_mode",
        "quiet_circulation_mode"
    },
    value = "idle_off"
}
