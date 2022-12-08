local tests = require( "tests" )
local zwave = require( "zwave" )

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/generic_door_lock_fixture")()

zwave.request_node_values = tests.create_mock()
zwave.synchronize_device_time = tests.create_mock()

local function test_impl(device_info)
    fixture:create_and_add_zwave_node(device_info)

    fixture:send_alarm( 0x26, 0x00 )

    fixture:send_alarm( 0x2A, 0x00 )
    fixture:send_alarm( 0x2A, 0x01 )

    fixture:send_alarm( 0x2B, 0x00 )
    fixture:send_alarm( 0x2B, 0x01 )

    fixture:send_alarm( 0x60, 0x00 )

    fixture:send_alarm( 0x61, 0x00 )

    fixture:send_alarm( 0x62, 0x00 )

    fixture:send_alarm( 0x63, 0x00 )

    fixture:send_alarm( 0x64, 0x00 )

    fixture:send_alarm( 0x65, 0x00 )

    fixture:send_alarm( 0x81, 0x00 )

    fixture:send_alarm( 0x82, 0x01 )
    fixture:send_alarm( 0x82, 0x00 )

    fixture:send_alarm( 0x83, 0x00 )

    fixture:send_alarm( 0xA1, 0x01 )
    fixture:send_alarm( 0xA1, 0x03 )

    fixture:send_alarm( 0xA7, 0x00 )

    fixture:send_alarm( 0xA8, 0x00 )

    fixture:send_alarm( 0xA9, 0x00 )
end

tests.run_with_params(
    test_impl,
    fixture:get_manufacturer_specifics()
)