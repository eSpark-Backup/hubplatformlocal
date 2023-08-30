local tests = require "tests"

local is_container_value_type = loadfile("HUB:zwave/scripts/helpers/is_container_value_type")

tests.assert_eq(is_container_value_type("dictionary.int"), true)
tests.assert_eq(is_container_value_type("array.string"), true)

tests.assert_eq(is_container_value_type("string"), false)
tests.assert_eq(is_container_value_type(""), false)
