local self = ...

return {
    label = {
        text = "Thermostat setback mode"
    },
    description = {
        text = "Thermostat setback mode"
    },
    value_type = "token",
    value = "unknown",
    value_default = "custom_value",
    enum = {
        unknown = {
            text = "Unknown"
        },
        custom_value = {
            text = "Custom Value"
        },
        frost_protection = {
            text = "Frost Protection"
        },
        energy_saving_mode = {
            text = "Energy Saving Mode"
        }
    }
}
