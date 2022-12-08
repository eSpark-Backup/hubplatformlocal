local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local fan_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

timer.set_timeout = tests.create_mock()

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


    for i, enum_value in ipairs(param.expected_enum) do
        zigbee.set_attribute_value = tests.create_mock()

        loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = enum_value} )

        tests.assert_called(
            zigbee.set_attribute_value,
            "check if attribute value seted",
            { { 42, 1, "fan_control", "FanMode", param.sequence[i] } } )

    end
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                sequence = {"low", "medium", "high"},
                expected_enum = {
                    fan_mode_enum.fanmode_on_low,
                    fan_mode_enum.fanmode_on_medium,
                    fan_mode_enum.fanmode_on_high
                }
            },
            description = "test with low/medium/high"
        },
        {
            value = {
                sequence = {"low", "high"},
                expected_enum = {
                    fan_mode_enum.fanmode_on_low,
                    fan_mode_enum.fanmode_on_high
                }
            },
            description = "test with low/high"
        },
        {
            value = {
                sequence = {"low", "medium", "high", "auto"},
                expected_enum = {
                    fan_mode_enum.fanmode_on_low,
                    fan_mode_enum.fanmode_on_medium,
                    fan_mode_enum.fanmode_on_high,
                    fan_mode_enum.fanmode_on_auto
                }
            },
            description = "test with low/medium/high/auto"
        },
        {
            value = {
                sequence = {"low", "high", "auto"},
                expected_enum = {
                    fan_mode_enum.fanmode_on_low,
                    fan_mode_enum.fanmode_on_high,
                    fan_mode_enum.fanmode_on_auto
                }
            },
            description = "test with low/high/auto"
        },
        {
            value = {
                sequence = {"on", "auto"},
                expected_enum = {
                    fan_mode_enum.fanmode_on,
                    fan_mode_enum.fanmode_on_auto
                }
            },
            description = "test with on/auto"
        }
    }
)
