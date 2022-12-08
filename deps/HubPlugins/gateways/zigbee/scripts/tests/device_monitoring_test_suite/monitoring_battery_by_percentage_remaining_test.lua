local tests = require( "tests" )
local core = require( "core" )

local fan_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

local function test_impl( param )
    local cluster_power_configuration = {
        name = "power_configuration",
        attributes = {
            {
                name = "BatteryPercentageRemaining",
                formatted_value = 200
            }
        }
    }

    local endpoint_1 = {
        id = 1,
        clusters = { cluster_power_configuration }
    }

    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Battery Device",
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
            item_name = "battery",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = "power_configuration",
            attribute_name = "BatteryPercentageRemaining"
        })

    tests.assert_ne(item_id, nil, "Try to find item id")

    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "power_configuration",
        attribute = {
            name = "BatteryPercentageRemaining",
            formatted_value = param.attr_value
        }
    } )

    if param.item_value then
        tests.assert_called(
            core.update_item_value,
            "check if item values updated",
            {{ item_id, param.item_value }}
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
    test_impl,
    {
        {
            value = {
                attr_value = "undefined"
            },
            description = "test with undefined"
        },
        {
            value = {
                attr_value = 0,
                item_value = 0
            },
            description = "test with 0"
        },
        {
            value = {
                attr_value = 20,
                item_value = 10
            },
            description = "test with 10"
        },
        {
            value = {
                attr_value = 100,
                item_value = 50
            },
            description = "test with 50"
        },
        {
            value = {
                attr_value = 160,
                item_value = 80
            },
            description = "test with 80"
        },
        {
            value = {
                attr_value = 200,
                item_value = 100
            },
            description = "test with 100"
        },
        {
            value = {
                attr_value = 220,
                item_value = 100
            },
            description = "test with >100"
        },
    }
)
