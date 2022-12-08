local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local cluster_door_lock = {
    name = "door_lock",
    attributes = {}
}

local endpoint_1 = {
    id = 1,
    clusters = {
        cluster_door_lock
    }
}

local node = {
    id = 42,
    manufacturer_id = "LUMI",
    model_id = "lumi.vibration.aq1",
    security = 0,
    is_reachable = 1,
    is_battery_powered = true,

    endpoints = { endpoint_1 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42
} )

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local activity_item_id = storage:call(
    "find_item_id",
    {
        item_name = "activity",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "",
    })

tests.assert_ne(activity_item_id, nil, "Try to find item id")

local bed_activity_item_id = storage:call(
    "find_item_id",
    {
        item_name = "bed_activity",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "",
    })

tests.assert_ne(bed_activity_item_id, nil, "Try to find item id")

-- check activity item
local function test_activity(params)
    core.update_item_value = tests.create_mock()
    timer.set_timeout = tests.create_mock()

    events_handler( {
        event = "unknown_attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_id = 0x0055,
        value = params.attr_value
    } )

    if params.item_value then
        tests.assert_called(
            core.update_item_value,
            "check if item values updated",
            {
                { activity_item_id, params.item_value }
            }
        )

        tests.assert_called(
            timer.set_timeout,
            "check if timer seted",
                {
                    {
                        60000,
                        "HUB:zigbee/scripts/timer_handlers/timeout/set_item_value",
                        {
                            node_id = 42,
                            item_id = activity_item_id,
                            item_value = "no_activity"
                        }
                    }
                }
        )
    else
       tests.assert_called(
            core.update_item_value,
            "check if item values not updated",
            {}
        )
    end
end

tests.run_with_params(
    test_activity,
    {
        {
            value = {
                attr_value = "1",
                item_value = "shake"
            },
            description = "test with shake"
        },
        {
            value = {
                attr_value = "2",
                item_value = "tilt"
            },
            description = "test with tilt"
        },
        {
            value = {
                attr_value = "3",
                item_value = "drop"
            },
            description = "test with drop"
        },
        {
            value = {
                attr_value = "4"
            },
            description = "test with unsupported"
        }
    }
)

-- check bed activity
for i=72, 214, 1 do
    core.update_item_value = tests.create_mock()
    events_handler( {
        event = "unknown_attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_id = 0x0503,
        value = tostring(i)
    } )

    tests.assert_called(
        core.update_item_value,
        "check if item values updated",
        {
            { bed_activity_item_id, i }
        }
    )
end
