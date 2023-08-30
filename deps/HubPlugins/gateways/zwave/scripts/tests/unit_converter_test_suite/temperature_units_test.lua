local tests = require("tests")

local round = loadfile("HUB:zwave/scripts/helpers/round")

local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")


do
    tests.assert_eq(unit_converter:call("convert", -15, "celsius", "fahrenheit"), 5)
    tests.assert_eq(unit_converter:call("convert", 0, "celsius", "fahrenheit"), 32)
    tests.assert_eq(unit_converter:call("convert", 15, "celsius", "fahrenheit"), 59)
    tests.assert_eq(unit_converter:call("convert", 5.5, "celsius", "fahrenheit"), 41.9)

    tests.assert_eq(unit_converter:call("convert", 20, "celsius", "celsius"), 20)
end

do
    tests.assert_eq(unit_converter:call("convert", -22, "fahrenheit", "celsius"), -30)
    tests.assert_eq(round(unit_converter:call("convert", 0, "fahrenheit", "celsius"), 2), -17.78)
    tests.assert_eq(unit_converter:call("convert", 50, "fahrenheit", "celsius"), 10)
    tests.assert_eq(round(unit_converter:call("convert", 50.5, "fahrenheit", "celsius"), 2), 10.28)

    tests.assert_eq(unit_converter:call("convert", 50, "fahrenheit", "fahrenheit"), 50)
end

do
    tests.assert_eq(unit_converter:call("convert_offset", -10, "celsius", "fahrenheit"), -18)
    tests.assert_eq(unit_converter:call("convert_offset", 5, "celsius", "fahrenheit"), 9)
    tests.assert_eq(unit_converter:call("convert_offset", 0, "celsius", "fahrenheit"), 0)

    tests.assert_eq(unit_converter:call("convert_offset", 20, "celsius", "celsius"), 20)
end

do
    tests.assert_eq(unit_converter:call("convert_offset", -18, "fahrenheit", "celsius"), -10)
    tests.assert_eq(unit_converter:call("convert_offset", 180, "fahrenheit", "celsius"), 100)
    tests.assert_eq(unit_converter:call("convert_offset", 0, "fahrenheit", "celsius"), 0)

    tests.assert_eq(unit_converter:call("convert_offset", 20, "fahrenheit", "fahrenheit"), 20)
end
