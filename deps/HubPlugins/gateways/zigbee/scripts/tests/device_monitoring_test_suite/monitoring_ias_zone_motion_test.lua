local tests = require( "tests" )
local core = require( "core" )

local cluster_ias_zone = {
    name = "ias_zone",
    attributes = {
        {
            name = "ZoneType",
            formatted_value = {zone_type = "motion_sensor"}
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
    model_id = "Motion Sensor",
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
        item_name = "motion",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
    })

tests.assert_ne(item_id, nil, "Try to find item id")

local security_item_id = storage:call(
    "find_item_id",
    {
        item_name = "motion",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = ".security_tag"
    })

tests.assert_ne(security_item_id, nil, "Try to find seciruty item id")

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

-- check motion detect
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_zone",
    attribute = {
        name = "ZoneStatus",
        formatted_value = { alarm_1 = "on", alarm_2 = "off", tamper = "not_tampered" }
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, true },
        { security_item_id, true },
        { tamper_item_id, "no_tampering_cover" }
    }
)

-- check stop motion detection
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_zone",
    attribute = {
        name = "ZoneStatus",
        formatted_value = { alarm_1 = "off", alarm_2 = "off", tamper = "not_tampered" }
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, false },
        { security_item_id, false },
        { tamper_item_id, "no_tampering_cover" }
    }
)

-- check motion detect
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_zone",
    attribute = {
        name = "ZoneStatus",
        formatted_value = { alarm_1 = "off", alarm_2 = "on", tamper = "not_tampered" }
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, true },
        { security_item_id, true },
        { tamper_item_id, "no_tampering_cover" }
    }
)

-- check stop motion detection
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_zone",
    attribute = {
        name = "ZoneStatus",
        formatted_value = { alarm_1 = "off", alarm_2 = "off", tamper = "not_tampered" }
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, false },
        { security_item_id, false },
        { tamper_item_id, "no_tampering_cover" }
    }
)

-- check tamper detection
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_zone",
    attribute = {
        name = "ZoneStatus",
        formatted_value = { alarm_1 = "off", alarm_2 = "off", tamper = "tampered" }
    }
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, false },
        { security_item_id, false },
        { tamper_item_id, "tampering_cover" }
    }
)
