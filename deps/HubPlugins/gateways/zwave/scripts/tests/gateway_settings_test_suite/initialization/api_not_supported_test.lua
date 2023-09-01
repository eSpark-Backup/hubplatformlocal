local core = require("core")

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

core.add_gateway_setting = nil
core.remove_gateway_setting = nil
core.set_gateway_setting_value = nil
core.set_gateway_setting_status = nil
core.get_gateway_setting_ids_by_gateway_id = nil
core.get_gateway_setting = nil

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()
