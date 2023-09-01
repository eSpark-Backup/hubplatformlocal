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
timer.cancel = tests.create_mock()
timer.set_timeout = tests.create_mock()

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
    generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",

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

local item_id = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.basic, 0 )
assert(item_id)

local function check_update(value)
    core.update_item_value = tests.create_mock()
    
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.basic,
        channel_id = 0,
        value = value
    } )
    
    tests.assert_called(
        core.update_item_value,
        "check updated",
        { { item_id, value } }
    )
end

check_update(5)
check_update(30)
check_update(50)
check_update(100)
