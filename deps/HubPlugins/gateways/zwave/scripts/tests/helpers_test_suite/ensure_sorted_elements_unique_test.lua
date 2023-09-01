local tests = require "tests"

local ensure_sorted_elements_unique = loadfile("HUB:zwave/scripts/helpers/ensure_sorted_elements_unique")

tests.assert_eq(
    ensure_sorted_elements_unique( { "a", "b", "c", "d", "e", "f" } ), 
    { "a", "b", "c", "d", "e", "f" }, 
    "check array without duplications" )

tests.assert_eq(
    ensure_sorted_elements_unique( { } ), 
    { }, 
    "check empty array" )

tests.assert_eq(
    ensure_sorted_elements_unique( { 1, 1, 1, 2, 3, 3 } ), 
    { 1, 2, 3 }, 
    "check array with duplications" )

local deep_equal = loadfile("HUB:zwave/scripts/helpers/deep_equal")

tests.assert_eq(
    ensure_sorted_elements_unique( { { "a", "b" }, { "a", "b" }, { "a", "c" } }, deep_equal ), 
    { { "a", "b" }, { "a", "c" } }, 
    "check array with duplications by comparator" )
