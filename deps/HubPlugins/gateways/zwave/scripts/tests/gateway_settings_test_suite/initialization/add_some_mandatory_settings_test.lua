local core = require("core")
local tests = require("tests")

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
loadfile( "HUB:zwave/scripts/startup" )()

local expectation = fixture.get_mandatory_settings_expectation()
fixture.check_gateway_settings(expectation)

core.set_gateway_setting_value("zwave.reachable_check_policy", "use_fixed_interval")
core.set_gateway_setting_value("zwave.wake_up_devices_polling_interval", 120)
expectation[3].value = "use_fixed_interval"
expectation[4].value = 120
fixture.check_gateway_settings(expectation)

core.remove_gateway_setting("zwave.mains_devices_polling_interval")
core.remove_gateway_setting("zwave.reachable_check_attempts_count")
local expectations_after_delete = {}
for i = 1, #expectation do
    if i ~= 1 and i ~= 2 then
        table.insert(expectations_after_delete, expectation[i])
    end
end
fixture.check_gateway_settings(expectations_after_delete)

-- emulate plugin starting second time
loadfile( "HUB:zwave/scripts/startup" )()

fixture.check_gateway_settings(expectation)
