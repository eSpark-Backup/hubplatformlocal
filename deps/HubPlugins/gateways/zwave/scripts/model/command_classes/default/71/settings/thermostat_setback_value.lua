local self = ...

return {
    label = {
        text = "Thermostat setback value"
    },
    description = {
        text = "Thermostat setback value"
    },
    value_type = "temperature",
    value = {
        value = 0,
        scale = "celsius",
        relative = true
    },
    value_min = {
        value = -12.8,
        scale = "celsius",
        relative = true
    },
    value_max = {
        value = 12.0,
        scale = "celsius",
        relative = true
    }
}
