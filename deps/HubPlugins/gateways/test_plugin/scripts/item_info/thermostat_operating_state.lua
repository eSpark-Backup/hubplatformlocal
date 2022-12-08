return {
    name = "thermostat_operating_state",
    value_type = "token",
    enum = {
        "idle",
        "heating",
        "cooling",
        "fan_only",
        "pending_heat",
        "pending_cool",
        "vent_economizer",
        "aux_heating",
        "2nd_stage_heating",
        "2nd_stage_cooling",
        "2nd_stage_aux_heat",
        "3rd_stage_aux_heat",
        "2nd_stage_fan",
        "3rd_stage_fan"
    },
    value = "idle",
    has_getter = true,
    has_setter = false
}
