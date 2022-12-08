local tests = require( "tests" )
local core = require( "core" )
local emergency_state = loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

local function test_impl(zone_type, params)
    local cluster_ias_zone = {
        name = "ias_zone",
        attributes = {
            {
                name = "ZoneType",
                formatted_value = {zone_type = zone_type}
            }
        }
    }

    local endpoint_1 = {
        id = 1,
        clusters = { cluster_ias_zone }
    }

    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Emergency",
        security = 0,
        is_reachable = 1,
        is_battery_powered = false,

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
    local item_id = storage:call(
        "find_item_id",
        {
            item_name = "emergency_state",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = "ias_zone",
            attribute_name = "ZoneStatus",
        })

    tests.assert_ne(item_id, nil, "Try to find item id")

    local security_item_id = storage:call(
        "find_item_id",
        {
            item_name = "emergency_state",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = "ias_zone",
            attribute_name = "ZoneStatus",
            tag = ".security_tag"
        })

    tests.assert_ne(security_item_id, nil, "Try to find security item id")

    local tamper_item_id = storage:call(
    "find_item_id",
    {
        item_name = "tampering_cover_alarm",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
    })

    tests.assert_ne(tamper_item_id, nil, "Try to find tamper item id")

    -- check
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute = {
            name = "ZoneStatus",
            formatted_value = { alarm_1 = params.alarm_1, alarm_2 = params.alarm_2, tamper = "not_tampered" }
        }
    } )

    tests.assert_called(
        core.update_item_value,
        "check if item values updated",
        {
            { item_id, params.item_value },
            { security_item_id, params.item_value ~= emergency_state.idle },
            { tamper_item_id, "no_tampering_cover" }
        } )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "remote_control",
            description = "test with remote control"
        },
        {
            value = "key_fob",
            description = "test with keyfob"
        },
        {
            value = "keypad",
            description = "test with keypad"
        }
    },
    {
        {
            value = {
                alarm_1 = "off",
                alarm_2 = "off",
                item_value = emergency_state.idle
            },
            description = "test with idle"
        },
        {
            value = {
                alarm_1 = "on",
                alarm_2 = "off",
                item_value = emergency_state.panic
            },
            description = "test with panic"
        },
        {
            value = {
                alarm_1 = "off",
                alarm_2 = "on",
                item_value = emergency_state.emergency
            },
            description = "test with emergency"
        },
        {
            value = {
                alarm_1 = "on",
                alarm_2 = "on",
                item_value = emergency_state.emergency_and_panic
            },
            description = "test with emergency and panic"
        }
    }
)
