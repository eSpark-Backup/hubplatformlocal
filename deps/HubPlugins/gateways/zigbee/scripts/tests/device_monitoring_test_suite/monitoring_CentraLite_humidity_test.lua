local tests = require( "tests" )
local core = require( "core" )

local function test_impl( model )
    local cluster_meter = {
        name = "temperature_measurement",
        attributes = {
            {
                name = "MeasuredValue",
                formatted_value = 0
            },
            {
                name = "MinMeasuredValue",
                formatted_value = "undefined"
            },
            {
                name = "MaxMeasuredValue",
                formatted_value = "undefined"
            }
        }
    }

    local cluster_humidity = {
        id = 0xFC45,
        name = "unknown",
        attributes = {
            {
                id = 0x00,
                name = "unknown",
                formatted_value = "0"
            },
            {
                id = 0x01,
                name = "unknown",
                formatted_value = "0xffff"
            },
            {
                id = 0x02,
                name = "unknown",
                formatted_value = "0xffff"
            }
        }
    }

    local endpoint_1 = {
        id = 1,
        clusters = { cluster_meter, cluster_humidity }
    }

    local node = {
        id = 42,
        manufacturer_id = "CentraLite",
        model_id = model,
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
    local item_id = storage:call(
        "find_item_id",
        {
            item_name = "humidity",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = 0xFC45,
            attribute_name = 0x00,
            tag = ".mfg_specific"
        })

    tests.assert_ne(item_id, nil, "Try to find item id")

    -- check undefined
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "mfg_specific_attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_id = 0xFC45,
        attribute = {
            name = "unknown",
            id = 0x00,
            formatted_value = "0xffff"
        }
    } )

    tests.assert_called( core.update_item_value, "check if item values not updated", {} )

    -- check 0 percent
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "mfg_specific_attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_id = 0xFC45,
        attribute = {
            name = "unknown",
            id = 0x00,
            formatted_value = "0"
        }
    } )

    tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 0, scale = "percent" } } } )

    -- check 50 percent
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "mfg_specific_attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_id = 0xFC45,
        attribute = {
            name = "unknown",
            id = 0x00,
            formatted_value = "5000"
        }
    } )

    tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 50, scale = "percent" } } } )

    -- check with borders
    core.update_item_value_with_min_max = tests.create_mock()

    cluster_humidity.attributes[2].formatted_value = "3000"
    cluster_humidity.attributes[3].formatted_value = "9500"

    events_handler( {
        event = "mfg_specific_attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_id = 0xFC45,
        attribute = {
            name = "unknown",
            id = 0x00,
            formatted_value = "8000"
        }
    } )

    tests.assert_called(
        core.update_item_value_with_min_max,
        "check if item values updated",
        {
            {
                item_id,
                { value = 80, scale = "percent" },
                { value = 30, scale = "percent" },
                { value = 95, scale = "percent" }
            }
        })
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "3310-G",
            description = "test with 3310-G model"
        },
        {
            value = "3310-Geu",
            description = "test with 3310-Geu model"
        }
    }
)
