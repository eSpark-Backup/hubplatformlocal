local test = require( "tests" )

local create_device_type_id = loadfile("HUB:zwave/scripts/helpers/create_device_type_id")

test.assert_eq(create_device_type_id(0, 0, 0), "0_0_0", "create_device_type_id from zeros")
test.assert_eq(create_device_type_id(122, 11, 74.0), "122_11_74", "create some id")
