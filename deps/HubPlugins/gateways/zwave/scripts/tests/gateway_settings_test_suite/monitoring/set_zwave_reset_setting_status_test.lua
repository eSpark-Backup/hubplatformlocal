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
            event = "module_reset",
            status = params.event_status
        })
        
    tests.assert_called(
        core.set_gateway_setting_status,
        "check setting status updated",
        { { "zwave.zwave_reset", params.status } }
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                event_status = "invoked",
                status = "pending"
            },
            description = "test with invoked"
        },
        {
            value = {
                event_status = "started",
                status = "pending"
            },
            description = "test with started"
        },
        {
            value = {
                event_status = "failed",
                status = "failed"
            },
            description = "test with failed"
        },
        {
            value = {
                event_status = "timeout",
                status = "failed"
            },
            description = "test with timeout"
        },
        {
            value = {
                event_status = "finished",
                status = "synced"
            },
            description = "test with finished"
        }
    }
)
