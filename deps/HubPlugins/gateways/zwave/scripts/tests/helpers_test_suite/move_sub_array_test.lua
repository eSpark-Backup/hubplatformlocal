local tests = require "tests"

local move_sub_array = loadfile("HUB:zwave/scripts/helpers/move_sub_array")

local array = { "a", "b", "c", "d", "e", "f" }

move_sub_array(array, 2, 2, 1)
tests.assert_eq(array, { "a", "b", "c", "d", "e", "f" }, "check moving element to itself")

move_sub_array(array, 2, 3, 3)
tests.assert_eq(array, { "a", "c", "d", "e", "b", "f" }, "check moving 3 elements down with intersecion")

move_sub_array(array, 3, 2, 3)
tests.assert_eq(array, { "a", "b", "c", "d", "e", "f" }, "check moving 3 elements up with intersecion")

move_sub_array(array, 1, 5, 2)
tests.assert_eq(array, { "e", "f", "a", "b", "c", "d" }, "check moving 2 elements down without intersecion")

move_sub_array(array, 5, 1, 2)
tests.assert_eq(array, { "a", "b", "c", "d", "e", "f" }, "check moving 2 elements up without intersecion")
