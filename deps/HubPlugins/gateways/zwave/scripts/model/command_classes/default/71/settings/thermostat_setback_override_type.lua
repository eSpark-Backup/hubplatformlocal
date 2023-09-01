local self = ...

return {
    label = {
        text = "Thermostat setback override type"
    },
    description = {
        text = "Thermostat setback override type"
    },
    value_type = "token",
    value = "unknown",
    value_default = "permanent_override",
    enum = {
        unknown = {
            text = "Unknown"
        },
        no_override = {
            text = "No Override"
        },
        temporary_override = {
            text = "Temporary Override"
        },
        permanent_override = {
            text = "Permanent Override"
        }
    }
}
