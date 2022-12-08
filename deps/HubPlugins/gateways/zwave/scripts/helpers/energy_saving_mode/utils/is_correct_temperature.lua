local self, item_value, expected_temp = ...

local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")

local calculation_error = item_value.scale == "fahrenheit" and 1 or 1.5

local old_value_in_farenheit = unit_converter:call(
    "convert",
    item_value.value,
    item_value.scale,
    "fahrenheit"
)

old_value_in_farenheit = hubplugins.weak_loadfile("HUB:zwave/scripts/utils/round_value")(old_value_in_farenheit, 1)

return math.abs(old_value_in_farenheit - expected_temp) <= calculation_error
