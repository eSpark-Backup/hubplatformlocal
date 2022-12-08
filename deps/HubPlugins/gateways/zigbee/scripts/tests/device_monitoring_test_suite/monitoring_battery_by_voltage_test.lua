local tests = require( "tests" )
local core = require( "core" )

local fan_mode_enum = require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

local function test_impl( params )
    local cluster_power_configuration = {
        name = "power_configuration",
        attributes = {
            {
                name = "BatteryVoltage",
                formatted_value = 30
            }
        }
    }

    for _, additional_attribute in ipairs(params.additional_attributes) do
        table.insert(cluster_power_configuration.attributes, additional_attribute)
    end

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
            attribute_name = "BatteryVoltage"
        })

    tests.assert_ne(item_id, nil, "Try to find item id")

    for attr_val, item_val in pairs(params.values) do
        core.update_item_value = tests.create_mock()

        events_handler( {
            event = "attribute_value_updated",
            node_id = 42,
            endpoint_id = 1,
            cluster_name = "power_configuration",
            attribute = {
                name = "BatteryVoltage",
                formatted_value = attr_val
            }
        } )

        if item_val ~= "undefined" then
            tests.assert_called(
                core.update_item_value,
                "check if item values updated",
                {{ item_id, item_val }}
            )
        else
            tests.assert_called(
                core.update_item_value,
                "check if item values not updated",
                {}
            )
        end
    end
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                additional_attributes = {},
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test without info about battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatterySize",
                        formatted_value = "AA"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test without info about battery quantity"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test without info about battery size"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 2
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "AA"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test with 2 batteries"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "AA"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [7.5] = 50,
                    [15] = 100,
                    [20] = 100
                }
            },
            description = "test with AA battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "AAA"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [7.5] = 50,
                    [15] = 100,
                    [20] = 100
                }
            },
            description = "test with AAA battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "C"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [7.5] = 50,
                    [15] = 100,
                    [20] = 100
                }
            },
            description = "test with C battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "D"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [7.5] = 50,
                    [15] = 100,
                    [20] = 100
                }
            },
            description = "test with D battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "CR2"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test with CR2 battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "CR123A"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test with CR123A battery"
        },
        {
            value = {
                additional_attributes = {
                    {
                        name = "BatteryQuantity",
                        formatted_value = 1
                    },
                    {
                        name = "BatterySize",
                        formatted_value = "other"
                    }
                },
                values = {
                    ["undefined"] = "undefined",
                    [0] = 0,
                    [15] = 50,
                    [30] = 100,
                    [35] = 100
                }
            },
            description = "test with unknown battery"
        },
    }
)
