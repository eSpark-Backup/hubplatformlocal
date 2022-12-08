local tests = require "tests"

local remove_if = loadfile("HUB:zwave/scripts/helpers/remove_from_array_if")

-- test empty array
local empty_array = {}
tests.assert_eq(remove_if(empty_array, function() error("predicate mustn't be executed") end ), 0, "nothing removed from empty array")
tests.assert_eq(empty_array, {}, "empty array has npt been changed")

-- remove 2 elements from not empty array
local some_array = { 1, 0, 60, 7, 2, 20 }
tests.assert_eq(remove_if(some_array, function(value) return value > 10 end ), 2, "2 elements removed from not empty array")
tests.assert_eq(some_array, { 1, 0, 7, 2 }, "array has been changed")

-- remove 1 element by index from not empty array
tests.assert_eq(remove_if(some_array, function(value, index) return index == 1 end ), 1, "1 element removed from not empty array")
tests.assert_eq(some_array, { 0, 7, 2 }, "array has been changed")

-- remove nothing elements from not empty array
tests.assert_eq(remove_if(some_array, function(value) return false end ), 0, "array has not been changed with negative predicate")
tests.assert_eq(some_array, { 0, 7, 2 }, "nothing removed from array after negative predicate")

-- remove all elements elements from not empty array
tests.assert_eq(remove_if(some_array, function(value) return true end ), 3, "array has been cleared")
tests.assert_eq(some_array, { }, "array is empty after all elements removing")
