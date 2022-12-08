local fixture = loadfile("HUB:zwave/scripts/tests/device_check_test_suite/fixture")()
local initializer = loadfile("HUB:zwave/scripts/check_device")
local zwave = require("zwave")
local tests = require "tests"
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

fixture.add_simple_device()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = assert(storage:call("find_device_id", 9000))

zwave.check_node_values = tests.create_mock()

local dimmer_item_id = storage:call("find_item_id", "dimmer", 9000, class_ids.dimmer, 0  )

initializer({ device_id = device_id, check = "values", items = {dimmer_item_id} })

tests.assert_called( 
    zwave.check_node_values, 
    "check node values requested",
    { { "9000", 200, { { channel_id = 0, class_id = 38 } } } }
)
    
tests.assert_eq(
    storage:call("find_device_checking_id", "9000"), 
    device_id, 
    "Check device checking id mapping"
)
