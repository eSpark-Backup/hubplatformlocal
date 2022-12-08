local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local timer = require( "timer" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )

zwave.request_node_values = tests.create_mock()

local cc_basic = { 
    id = class_ids.basic,
}

local channel_0 = { 
    id = 0,
    classes = { cc_basic }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_NOT_USED",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

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

local item_id = storage:call("find_item_id", item_names.switch, 9000, class_ids.basic, 0 )
assert(item_id)

local external_setter = hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")

local function check_set(value, value_expected)
    zwave.set_value = tests.create_mock()
    zwave.request_value = tests.create_mock()
    
    external_setter({item_id = item_id, value = value})
    
    tests.assert_called(
        zwave.set_value,
        "check seted",
        { { "9000", class_ids.basic, 0, value_expected } }
    )
    
    tests.assert_called(
        zwave.request_value,
        "check requested",
        { { "9000", class_ids.basic, 0 } }
    )
end

check_set(false, 0)
check_set(true, 0xFF)
