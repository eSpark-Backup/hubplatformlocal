local core = require("core")
local tests = require("tests")

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

fixture.check_gateway_settings(fixture.get_mandatory_settings_expectation())

-- emulate plugin starting second time
core.add_gateway_setting = tests.create_mock()
loadfile( "HUB:zwave/scripts/startup" )()

tests.assert_called(
    core.add_gateway_setting,
    "check no additional settings added",
    {}
)
