local zwave = require("zwave")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local core = require("core")
local timer = require("timer")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

zwave.set_value = tests.create_mock()
zwave.request_value = tests.create_mock()

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

local item_id = storage:call("find_item_id", item_names.switch, "9000", class_ids.switch_binary, channel_0.id)
tests.assert_ne(item_id, nil, "check item id")

local external_setter = hubplugins.weak_loadfile( "HUB:zwave/scripts/set_item_value" )

-- set true value from ui
do 
    external_setter({
        item_id = item_id,
        value = true
    })

    tests.assert_called(
        zwave.set_value,
        "set switch binary",
        { { "9000", class_ids.switch_binary, channel_0.id, true } }
    )

    tests.assert_called(
        zwave.request_value,
        "request switch binary",
        { { "9000", class_ids.switch_binary, channel_0.id } }
    )

    tests.assert_eq(timer.get_count(), 0, "check no timers")
end

-- set true received from znet
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = channel_0.id,
        class_id = class_ids.switch_binary,
        value = 0xFF
    })

    tests.assert_eq(core.get_item(item_id).value, true, "check core value changed")

    tests.assert_eq(timer.get_count(), 0, "check no timers")
end

-- set false value from ui
do
    external_setter({
        item_id = item_id,
        value = false
    })

    tests.assert_called(
        zwave.set_value,
        "set switch binary",
        { { "9000", class_ids.switch_binary, channel_0.id, false } }
    )

    tests.assert_called(
        zwave.request_value,
        "request switch binary",
        { { "9000", class_ids.switch_binary, channel_0.id } }
    )

    tests.assert_eq(timer.get_count(), 0, "check no timers")
end

-- set true received from znet with target value and duration
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = channel_0.id,
        class_id = class_ids.switch_binary,
        value = 0xF0,
        target_value = 0x00,
        target_duration = 8000
    })

    tests.assert_eq(core.get_item(item_id).value, true, "check core value not changed")

    timer.add_time(7000)

    tests.assert_called( zwave.request_value, "request switch binary not called", { } )

    timer.add_time(1000)

    tests.assert_called(
        zwave.request_value,
        "request switch binary",
        { { "9000", class_ids.switch_binary, channel_0.id } }
    )
end


-- set true received from znet with target value without duration
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = channel_0.id,
        class_id = class_ids.switch_binary,
        value = 0xF0,
        target_value = 0x00
    })

    tests.assert_eq(core.get_item(item_id).value, true, "check core value not changed")

    timer.add_time(2000)

    tests.assert_called( zwave.request_value, "request switch binary not called", { } )

    timer.add_time(1000)

    tests.assert_called(
        zwave.request_value,
        "request switch binary",
        { { "9000", class_ids.switch_binary, channel_0.id } }
    )
end