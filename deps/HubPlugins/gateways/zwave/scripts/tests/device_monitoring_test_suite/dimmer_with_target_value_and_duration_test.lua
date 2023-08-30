local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )

zwave.request_node_values = tests.create_mock()
zwave.set_value = tests.create_mock()
zwave.request_value = tests.create_mock()

local timer = require( "timer" )

local channel_0 = { 
    id = 0,
    classes = { 
        { id = class_ids.dimmer }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
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

local item_id = storage:call("find_item_id", "dimmer", 9000, class_ids.dimmer, 0 )
local estimated_value_timer_id = hubplugins.object("estimated_value_timer", "HUB:zwave/scripts/timer_handlers", item_id)._timer_id
local request_value_timer_id = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0, class_ids.dimmer)._timer_id

local function get_item_value()
    return core.get_item( item_id ).value
end

do
    set_item_value({ item_id = item_id, value = 20 })

    tests.assert_eq( timer.get_time_to_execution(request_value_timer_id), 1000, "check request value timer enabled after set_item_value" )
    tests.assert_called( zwave.set_value, "check zwave.set_value called after set_item_value", nil, 1 )
    tests.assert_called( zwave.request_value, "check zwave.request_value not called after set_item_value", nil, 0 )

    timer.add_time(1000)

    tests.assert_called( zwave.request_value, "check zwave.request_value called after set_item_value and timeout", nil, 1 )
end

-- send dimmer basic event
local function send_and_check_value_updated_event(value, target_value, target_duration, values)
    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.dimmer,
        value = value,
        target_value = target_value,
        target_duration = target_duration
    } )

    tests.assert_eq( timer.get_time_to_execution(request_value_timer_id), 10000, "check request value timer enabled" )

    tests.assert_eq(get_item_value(), value, "check first value")

    for _, value in ipairs(values) do
        timer.add_time(1000)

        tests.assert_eq(get_item_value(), value, "check estimated value")
    end

    timer.add_time(10000 - 1000 * #values)

    tests.assert_called( zwave.request_value, "check zwave.request_value called", nil, 1 )
    tests.assert_eq( timer.get_count(), 0, "check not timers after finish" )
end

send_and_check_value_updated_event( 30, 60, 3000, { 40, 50, 60 } )
send_and_check_value_updated_event( 10, 24, 3500, { 14, 18, 22, 24 } )
send_and_check_value_updated_event( 0, 5, 100, { 5 } )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",     
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3
} )
