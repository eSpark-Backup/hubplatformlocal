local tests = require( "tests" )
local core = require( "core" )

local fan_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

local function test_impl( param )
    local cluster_fan_control = {
        name = "fan_control",
        attributes = {
            {
                name = "FanMode",
                formatted_value = "off"
            },
            {
                name = "FanModeSequence",
                formatted_value = param.sequence
            }
        }
    }

    local endpoint_1 = {
        id = 1,
        clusters = { cluster_fan_control }
    }

    local node = {
        id = 42,
        manufacturer_id = "Ezlo",
        model_id = "Thermostat",
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
            item_name = "thermostat_fan_mode",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = "fan_control",
            attribute_name = "FanMode"
        })

    tests.assert_ne(item_id, nil, "Try to find item id")


    for i, attr_value in ipairs(param.sequence) do
        core.update_item_value = tests.create_mock()

        events_handler( {
            event = "attribute_value_updated",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = "fan_control",
            attribute = {
                name = "FanMode",
                formatted_value = attr_value
            }
        } )

        tests.assert_called(
            core.update_item_value,
            "check if item values updated",
            {{ item_id, param.expected_enum[i] }}
        )
    end
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                sequence = {"low", "medium", "high"},
                expected_enum = {
                    fan_mode_enum.low,
                    fan_mode_enum.medium,
                    fan_mode_enum.high
                }
            },
            description = "test with low/medium/high"
        },
        {
            value = {
                sequence = {"low", "high"},
                expected_enum = {
                    fan_mode_enum.low,
                    fan_mode_enum.high
                }
            },
            description = "test with low/high"
        },
        {
            value = {
                sequence = {"low", "medium", "high", "auto"},
                expected_enum = {
                    fan_mode_enum.low,
                    fan_mode_enum.medium,
                    fan_mode_enum.high,
                    fan_mode_enum.auto
                }
            },
            description = "test with low/medium/high/auto"
        },
        {
            value = {
                sequence = {"low", "high", "auto"},
                expected_enum = {
                    fan_mode_enum.low,
                    fan_mode_enum.high,
                    fan_mode_enum.auto
                }
            },
            description = "test with low/high/auto"
        },
        {
            value = {
                sequence = {"on", "auto"},
                expected_enum = {
                    fan_mode_enum.on,
                    fan_mode_enum.auto
                }
            },
            description = "test with on/auto"
        }
    }
)
