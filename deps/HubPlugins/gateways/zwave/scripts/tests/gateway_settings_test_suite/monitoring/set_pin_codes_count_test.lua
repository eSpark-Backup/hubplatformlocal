local tests = require "tests"
local zwave = require "zwave"
local core = require "core"

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

core.set_gateway_setting_value = tests.create_mock()

loadfile( "HUB:zwave/scripts/set_gateway_setting_value" )(
    {
        setting_id = "zwave.unset_pin_codes_count_to_stop_requesting",
        value = 10
    })
    
tests.assert_called(
    core.set_gateway_setting_value,
    "check setting value updated",
    { { "zwave.unset_pin_codes_count_to_stop_requesting", 10 } }
)
