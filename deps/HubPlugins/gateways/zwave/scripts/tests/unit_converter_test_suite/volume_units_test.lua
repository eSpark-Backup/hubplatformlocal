local tests = require("tests")

local round = loadfile("HUB:zwave/scripts/helpers/round")

local unit_converter = hubplugins.object("unit_converter", "HUB:zwave/scripts/utils")


do
    tests.assert_eq(round(unit_converter:call("convert", 10, "cubic_meter", "cubic_feet"), 4), 353.1467)
    tests.assert_eq(round(unit_converter:call("convert", 0.1, "cubic_meter", "gallons"), 4), 26.4172)
    tests.assert_eq(unit_converter:call("convert", 0.5, "cubic_meter", "liter"), 500)
    tests.assert_eq(unit_converter:call("convert", 0.5, "cubic_meter", "cubic_meter"), 0.5)

    tests.assert_eq(unit_converter:call("convert", 0.5, "cubic_meter", "wrong_scale"), nil)
end

do
    tests.assert_eq(unit_converter:call("convert", 10, "cubic_feet", "cubic_feet"), 10)
    tests.assert_eq(round(unit_converter:call("convert", 2, "cubic_feet", "gallons"), 4), 14.9610)
    tests.assert_eq(round(unit_converter:call("convert", 0.5, "cubic_feet", "liter"), 4), 14.1584)
    tests.assert_eq(round(unit_converter:call("convert", 20, "cubic_feet", "cubic_meter"), 4), 0.5663)

    tests.assert_eq(unit_converter:call("convert", 0.5, "cubic_feet", "wrong_scale"), nil)
end

do
    tests.assert_eq(round(unit_converter:call("convert", 10, "gallons", "cubic_feet"), 4), 1.3368)
    tests.assert_eq(unit_converter:call("convert", 2, "gallons", "gallons"), 2)
    tests.assert_eq(round(unit_converter:call("convert", 0.5, "gallons", "liter"), 4), 1.8927)
    tests.assert_eq(round(unit_converter:call("convert", 20, "gallons", "cubic_meter"), 4), 0.0757)

    tests.assert_eq(unit_converter:call("convert", 0.5, "gallons", "wrong_scale"), nil)
end

do
    tests.assert_eq(round(unit_converter:call("convert", 1, "liter", "cubic_feet"), 4), 0.0353)
    tests.assert_eq(round(unit_converter:call("convert", 100, "liter", "gallons"), 4), 26.4172)
    tests.assert_eq(unit_converter:call("convert", 0.5, "liter", "liter"), 0.5)
    tests.assert_eq(unit_converter:call("convert", 200, "liter", "cubic_meter"), 0.2)

    tests.assert_eq(unit_converter:call("convert", 0.5, "liter", "wrong_scale"), nil)
end
