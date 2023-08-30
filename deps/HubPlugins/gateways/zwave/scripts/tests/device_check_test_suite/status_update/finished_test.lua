local fixture = loadfile("HUB:zwave/scripts/tests/device_check_test_suite/fixture")()
local initializer = loadfile("HUB:zwave/scripts/check_device")
local events_handler = loadfile("HUB:zwave/scripts/events/events_handling")
local zwave = require("zwave")
local tests = require "tests"
local core = require "core"

fixture.add_simple_device()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = assert(storage:call("find_device_id", 9000))

local function test_impl(params)
    zwave.check_reachable_status = tests.create_mock()
    initializer({ device_id = device_id, check = "status" })
    
    core.send_device_checked_broadcast = tests.create_mock()
    
    events_handler( { event = "node_check_status", node_id = 9000, status = params.event_status } )
    
    tests.assert_called( 
        core.send_device_checked_broadcast, 
        "check broadcast sent",
        { { { device_id = device_id, status = params.broadcast_status } } }
    )
    
    tests.assert_eq(
        storage:call("find_device_checking_id", "9000"), 
        nil, 
        "Check device checking id mapping"
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                event_status = "finished",
                broadcast_status = "finished",
            },
            description = "test with finished"
        },
        {
            value = {
                event_status = "failed",
                broadcast_status = "failed",
            },
            description = "test with failed"
        },
        {
            value = {
                event_status = "timeout",
                broadcast_status = "failed",
            },
            description = "test with timeout"
        }
    }
)
