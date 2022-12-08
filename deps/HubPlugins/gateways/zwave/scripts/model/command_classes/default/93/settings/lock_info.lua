local self, value = ...

return {
    name = "Lock info",
    description =  "The device is locked. See detailes and manufacturer contact information to unlock it.",
    value_type = "dictionary.string",
    has_setter = false
}
