local tests = require "tests"
local zwave = require "zwave"
local core = require "core"

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
zwave.set_devices_poll_intervals = tests.create_mock()
loadfile( "HUB:zwave/scripts/startup" )()

tests.assert_called(
    zwave.set_devices_poll_intervals,
    "check polling intervals sended to zwave",
    { { false, 5400, 10800, 3 } }
)

core.set_gateway_setting_value("zwave.mains_devices_polling_interval", 10700)
core.set_gateway_setting_value("zwave.reachable_check_attempts_count", 2)
core.set_gateway_setting_value("zwave.reachable_check_policy", "use_fixed_interval")
core.set_gateway_setting_value("zwave.wake_up_devices_polling_interval", 5300)

-- emulate plugin starting second time
zwave.set_devices_poll_intervals = tests.create_mock()
loadfile( "HUB:zwave/scripts/startup" )()

tests.assert_called(
    zwave.set_devices_poll_intervals,
    "check polling intervals sended to zwave",
    { { true, 5300, 10700, 2 } }
)
