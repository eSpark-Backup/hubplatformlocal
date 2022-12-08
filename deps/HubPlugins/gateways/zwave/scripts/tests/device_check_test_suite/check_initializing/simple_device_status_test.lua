local fixture = loadfile("HUB:zwave/scripts/tests/device_check_test_suite/fixture")()
local initializer = loadfile("HUB:zwave/scripts/check_device")
local zwave = require("zwave")
local tests = require "tests"

fixture.add_simple_device()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = assert(storage:call("find_device_id", 9000))

zwave.check_reachable_status = tests.create_mock()

initializer({ device_id = device_id, check = "status" })

tests.assert_called( 
    zwave.check_reachable_status, 
    "check reachable status requested",
    { {"9000"} }
)
    
tests.assert_eq(
    storage:call("find_device_checking_id", "9000"), 
    device_id, 
    "Check device checking id mapping"
)
