local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local helper = {}

helper.attribute_to_item_name = {
    RMSVoltage = item_names.electric_meter_volt,
    RMSCurrent = item_names.electric_meter_amper,
    ActivePower = item_names.electric_meter_watt,
}

helper.attribute_to_scale = {
    RMSVoltage = "volt",
    RMSCurrent = "ampere",
    ActivePower = "watt",
}

helper.value_modification_attributes = {
    RMSVoltage = { divisor = "ACVoltageDivisor", multiplier = "ACVoltageMultiplier" },
    RMSCurrent = { divisor = "ACCurrentDivisor", multiplier = "ACCurrentMultiplier" },
    ActivePower = { divisor = "ACPowerDivisor", multiplier = "ACPowerMultiplier" }
}

return helper
