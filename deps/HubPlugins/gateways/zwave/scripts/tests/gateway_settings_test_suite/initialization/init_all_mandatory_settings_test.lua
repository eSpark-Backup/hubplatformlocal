local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

fixture.check_gateway_settings(fixture.get_mandatory_settings_expectation())
