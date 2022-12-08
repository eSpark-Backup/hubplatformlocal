local self = ...

return {
    label = {
        text = "Manufacturer specific thermostat mode data"
    },
    description = {
        text = "A string of 0..7 bytes encoded in hex format. E.g.: 5f0102 setting value corresponds to 3 bytes: 0x5f, 0x01, 0x02"
    },
    value_type = "string",
    value = "",
    value_default = "",
    status = "synced",
    has_setter = true
}
