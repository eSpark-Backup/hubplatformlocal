return {
    name = "thermostat_energy_saving_mode",
    value_type = "token",
    enum = {
        "off",
        "low",
        "moderate",
        "aggressive",
        "low_postponed",
        "moderate_postponed",
        "aggressive_postponed"
    },
    value = "off",
    has_getter = true,
    has_setter = true
}
