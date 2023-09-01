local fixture = loadfile("HUB:zwave/scripts/tests/device_check_test_suite/fixture")()
local initializer = loadfile("HUB:zwave/scripts/check_device")
local zwave = require("zwave")
local tests = require "tests"

fixture.add_simple_device()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = assert(storage:call("find_device_id", 9000))

zwave.check_node_values = tests.create_mock()

initializer({ device_id = device_id, check = "values" })

tests.assert_called( 
    zwave.check_node_values, 
    "check node values requested",
    function(call_index, node_id, delay, cc_to_check)
        tests.assert_eq(node_id, "9000")
        tests.assert_eq(delay, 200)
        tests.assert_eq(#cc_to_check, 2)
        for _, cc_info in ipairs(cc_to_check) do
            tests.assert_eq(cc_info.channel_id, 0)
            assert( cc_info.class_id == 38 or cc_info.class_id == 37 )
        end
    end, 
    1
)
    
tests.assert_eq(
    storage:call("find_device_checking_id", "9000"), 
    device_id, 
    "Check device checking id mapping"
)
