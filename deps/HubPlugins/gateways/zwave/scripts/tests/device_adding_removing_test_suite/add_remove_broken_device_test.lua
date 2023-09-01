local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require("core")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { { id = class_ids.switch_binary }, }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- make device broken
local function callback(device)
    core.update_device_status(device.id, "broken")
end
loadfile("HUB:zwave/scripts/helpers/for_each_device")(9000, callback)

local expected_devices = {
    {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin",
        node_id = "9000",
        battery_powered = true,
        items = {
            {
                name = item_names.switch,
                cc_id = class_ids.switch_binary,
                channel_id = 0,
            }
        },
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
