local self = ...

return {
    label = {
        text = "Enable alarm for tilt event"
    },
    description = {
        text = "Initiate alarm when dorr or window tilted"
    },
    value_type = "bool",
    value = true,
    value_default = true,
    status = "synced",
    has_setter = true
}
