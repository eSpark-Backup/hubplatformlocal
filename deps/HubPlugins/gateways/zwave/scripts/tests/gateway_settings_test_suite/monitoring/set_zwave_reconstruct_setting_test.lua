local tests = require "tests"
local zwave = require "zwave"
local core = require "core"

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
zwave.set_devices_poll_intervals = tests.create_mock()
loadfile( "HUB:zwave/scripts/startup" )()

zwave.reconstruct_network = tests.create_mock()
core.set_gateway_setting_value = tests.create_mock()

loadfile( "HUB:zwave/scripts/set_gateway_setting_value" )(
    {
        setting_id = "zwave.zwave_reconstruct"
    })
    
tests.assert_called(
    core.set_gateway_setting_value,
    "check no setting value updated",
    {}
)

tests.assert_called(
    zwave.reconstruct_network,
    "check zwave reconstruct executed",
    1
)
