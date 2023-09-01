local tests = require "tests"

local is_array = loadfile("HUB:zwave/scripts/helpers/is_array")

tests.assert_eq(is_array({}), false, "check empty table")
tests.assert_eq(is_array(nil), false, "check nil")
tests.assert_eq(is_array(1), false, "check not table")
tests.assert_eq(is_array({ nil, "x"}), false, "check not contiguous array")

tests.assert_eq(is_array({ 1 }), true, "check array with one element")
tests.assert_eq(is_array({ 1, "1", {} }), true, "check array with different elements")
tests.assert_eq(is_array({ [3] = "x", [2] = "y", [1] = "z" }), true, "check array created by keys")

tests.assert_eq(is_array({ [0] = "x" }), false, "check table with zero key")
tests.assert_eq(is_array({ [0] = "x", [1] = "y" }), false, "check table with '0' and '1' keys")
tests.assert_eq(is_array({ [2] = "x", [1] = "y", [4] = "z" }), false, "check table 1,2,4 (sparsed array)")
tests.assert_eq(is_array({ [2] = "x", [3] = "y" }), false, "check absent first element in table")
tests.assert_eq(is_array({ [1] = "x", [2.1] = "y" }), false, "check table with float key")
tests.assert_eq(is_array({ [1] = "x", ["2"] = "y" }), false, "check table with string key")


local indexes = {}
for i = 1, 2500 do
    table.insert(indexes, i)
end

math.randomseed(314)
loadfile("TEST:scripts/utils/shuffle_array")(indexes)

local big_array = {}
for _, index in ipairs(indexes) do
    big_array[index] = "val"
end

tests.assert_eq(is_array(big_array), true, "check big array fillen in random order")

big_array[2499] = nil

tests.assert_eq(is_array(big_array), false, "check broken big array")
