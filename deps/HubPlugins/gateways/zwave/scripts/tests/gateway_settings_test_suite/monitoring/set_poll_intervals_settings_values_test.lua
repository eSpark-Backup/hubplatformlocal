local tests = require "tests"
local zwave = require "zwave"
local core = require "core"

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
zwave.set_devices_poll_intervals = tests.create_mock()
loadfile( "HUB:zwave/scripts/startup" )()

local function test_impl(params)
    zwave.set_devices_poll_intervals = tests.create_mock()
    core.set_gateway_setting_value = tests.create_mock()
    
    loadfile( "HUB:zwave/scripts/set_gateway_setting_value" )(
        {
            setting_id = params.setting_id,
            value = params.value
        })
        
    tests.assert_called(
        core.set_gateway_setting_value,
        "check setting value updated",
        { { params.setting_id, params.value } }
    )
    
    tests.assert_called(
        zwave.set_devices_poll_intervals,
        "check poll intervals updated in zwave",
        { params.zwave_poll_intervals }
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                setting_id = "zwave.mains_devices_polling_interval",
                value = 5000,
                zwave_poll_intervals = { false, 5400, 5000, 3 }
            },
            description = "test with mains device polling interval"
        },
        {
            value = {
                setting_id = "zwave.reachable_check_attempts_count",
                value = 5,
                zwave_poll_intervals = { false, 5400, 10800, 5 }
            },
            description = "test with attempts count"
        },
        {
            value = {
                setting_id = "zwave.wake_up_devices_polling_interval",
                value = 7000,
                zwave_poll_intervals = { false, 7000, 10800, 3 }
            },
            description = "test with wake up device polling interval"
        },
        {
            value = {
                setting_id = "zwave.reachable_check_policy",
                value = "default",
                zwave_poll_intervals = { false, 5400, 10800, 3 }
            },
            description = "test with policy default"
        },
        {
            value = {
                setting_id = "zwave.reachable_check_policy",
                value = "use_fixed_interval",
                zwave_poll_intervals = { true, 5400, 10800, 3 }
            },
            description = "test with policy use_fixed_interval"
        }
    }
)
