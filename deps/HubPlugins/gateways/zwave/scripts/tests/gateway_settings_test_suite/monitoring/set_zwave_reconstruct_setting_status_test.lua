local tests = require "tests"
local zwave = require "zwave"
local core = require "core"
local events_handler = loadfile( "HUB:zwave/scripts/model/gateway/events_handling/events_handling" )

local fixture = loadfile( "HUB:zwave/scripts/tests/gateway_settings_test_suite/fixtures/base_fixture" )()
fixture.init()

-- emulate plugin starting
zwave.set_devices_poll_intervals = tests.create_mock()
loadfile( "HUB:zwave/scripts/startup" )()

local function test_impl(params)
    core.set_gateway_setting_status = tests.create_mock()
    
    events_handler(
        {
            event = params.event,
        })
        
    tests.assert_called(
        core.set_gateway_setting_status,
        "check setting status updated",
        { { "zwave.zwave_reconstruct", params.status } }
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                event = "reconstruct_started",
                status = "pending"
            },
            description = "test with reconstruct_started"
        },
        {
            value = {
                event = "reconstruct_finished",
                status = "synced"
            },
            description = "test with reconstruct_finished"
        },
        {
            value = {
                event = "reconstruct_finished_error",
                status = "failed"
            },
            description = "test with reconstruct_finished_error"
        }
    }
)
