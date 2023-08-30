local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

-- prepare and add nodes

for node_id = 1, 5 do -- multilevel switches
    local node = {
        id = tostring(node_id),
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",

        security = { "none" },
        security_class_started = { "none" },

        channels = { }
    }

    for channel_id = 0, 4 do
        table.insert( node.channels, { 
            id = channel_id,
            classes = {  
                { id = class_ids.dimmer },
                { id = class_ids.basic }
            }
        } )
    end

    zwave.add_node( node )

    events_handler( {
        event = "node_added",
        node_id = node_id 
    } )
end

for node_id = 11, 15 do -- door locks
    local node = {
        id = tostring(node_id),
        manufacturer_id = 0,
        product_type = 1,
        product_id = 20,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { 
            {  
                id = 0,
                classes = {  
                    { id = class_ids.door_lock }
                }
            } 
        }
    }

    zwave.add_node( node )

    events_handler( {
        event = "node_added",
        node_id = node_id 
    } )
end

for node_id = 21, 25 do -- basic cc based multilevel switches
    local node = {
        id = tostring(node_id),
        manufacturer_id = 0,
        product_type = 1,
        product_id = 200,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",

        security = { "none" },
        security_class_started = { "none" },

        channels = { 
            {  
                id = 0,
                classes = {  
                    { id = class_ids.basic }
                }
            } 
        }
    }

    zwave.add_node( node )

    events_handler( {
        event = "node_added",
        node_id = node_id 
    } )
end


zwave.set_value_multicast = tests.create_mock()
zwave.set_value = tests.create_mock()

-- call for one node
do
    local dimmer_up_item_id = assert( storage:call("find_item_id", item_names.dimmer, 3, class_ids.dimmer, 1 ))

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value_multicast")({
        item_ids = { dimmer_up_item_id },
        value = 22
    })

    tests.assert_called(zwave.set_value, "check set value for single dimmer", { { "3", class_ids.dimmer, 1, 22 } })
    tests.assert_called_unordered(zwave.set_value_multicast, "check set value multicast not called for one node", {})
end

-- split by channel and cc
do
    local function node_and_channel_checking_rule(node_id, channel_id)
        return 5 - channel_id ~= node_id
    end

    local dimmer_item_ids = {  }
    for node_id = 1, 5 do
        for channel_id = 0, 4 do
            if node_and_channel_checking_rule(node_id, channel_id) then
                local item_id = assert( storage:call("find_item_id", item_names.dimmer, node_id, class_ids.dimmer, channel_id ))
                table.insert(dimmer_item_ids, item_id)
            end
        end
    end

    for  node_id = 21, 25 do
        local item_id = assert( storage:call("find_item_id", item_names.dimmer, node_id, class_ids.basic, 0 ))
        table.insert(dimmer_item_ids, item_id)
    end

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value_multicast")({
        item_ids = dimmer_item_ids,
        value = 10
    })

    -- check expectations

    tests.assert_called_unordered( 
        zwave.set_value_multicast, 
        "check multicast called", 
        {
            { { 1, 2, 3, 4 }, class_ids.dimmer, 0, 10, "chip_singlecasts" },
            { { 1, 2, 3, 5 }, class_ids.dimmer, 1, 10, "chip_singlecasts" },
            { { 1, 2, 4, 5 }, class_ids.dimmer, 2, 10, "chip_singlecasts" },
            { { 1, 3, 4, 5 }, class_ids.dimmer, 3, 10, "chip_singlecasts" },
            { { 2, 3, 4, 5 }, class_ids.dimmer, 4, 10, "chip_singlecasts" },
            { { 21, 22, 23, 24, 25 }, class_ids.basic, 0, 10, "chip_singlecasts" },
        }
    )
end

-- split by value
do
    local dimmer_up_item_id_1 = assert( storage:call("find_item_id", item_names.dimmer_up, 1, class_ids.dimmer, 0 ))
    local dimmer_up_item_id_2 = assert( storage:call("find_item_id", item_names.dimmer_up, 2, class_ids.dimmer, 0 ))

    local dimmer_down_item_id_1 = assert( storage:call("find_item_id", item_names.dimmer_down, 3, class_ids.dimmer, 0 ))
    local dimmer_down_item_id_2 = assert( storage:call("find_item_id", item_names.dimmer_down, 4, class_ids.dimmer, 0 ))

    local dimmer_item_id = assert( storage:call("find_item_id", item_names.dimmer, 5, class_ids.dimmer, 0 ))

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value_multicast")({
        item_ids = { dimmer_up_item_id_1, dimmer_down_item_id_1, dimmer_up_item_id_2, dimmer_down_item_id_2, dimmer_item_id },
        value = 20
    })

    tests.assert_called_unordered( 
        zwave.set_value_multicast, 
        "check multicast called", 
        {
            { { 1, 2 }, class_ids.dimmer, 0, 0x63, "chip_singlecasts" },
            { { 3, 4 }, class_ids.dimmer, 0, 0, "chip_singlecasts" }
        }
    )

    tests.assert_called(zwave.set_value, "check set value for single dimmer", { { "5", class_ids.dimmer, 0, 20 } })
end


-- send singlecasts if multicast not supported
do
    zwave.set_door_lock_mode = tests.create_mock()

    local door_lock_item_id_1 = assert( storage:call("find_item_id", item_names.door_lock, 11, class_ids.door_lock, 0 ))
    local door_lock_item_id_2 = assert( storage:call("find_item_id", item_names.door_lock, 12, class_ids.door_lock, 0 ))
    local door_lock_item_id_3 = assert( storage:call("find_item_id", item_names.door_lock, 13, class_ids.door_lock, 0 ))

    hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value_multicast")({
        item_ids = { door_lock_item_id_1, door_lock_item_id_2, door_lock_item_id_3},
        value = "secured"
    })

    tests.assert_called_unordered(zwave.set_door_lock_mode, "check set door lock states single casts", 
    {
        { "11", 0, "secured" },
        { "12", 0, "secured" },
        { "13", 0, "secured" }
    })

    tests.assert_called(zwave.set_value, "check default set value not called", {})
    tests.assert_called_unordered(zwave.set_value_multicast, "check set value multicast not called for not supported class id", {})
end