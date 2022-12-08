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

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "5.1",
                        "5.1"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.38"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 7
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                {
                                    endpoint_id = 0,
                                    node_id = 1
                                }
                            }
                        }
                    },
                    association_groups_count = 8,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                },
                {
                    id = 50,
                    reset_supported = true,
                    scales = {
                        "kilo_watt_hour",
                        "watt"
                    },
                    type = "electric",
                    version = 4
                }
            },
            id = 2
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 2
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius"
                            },
                            sensor_type = "air_temperature"
                        }
                    },
                    version = 7
                }
            },
            id = 3
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 345,
    product_id = 81,
    product_type = 2,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- channel 0 switch updated
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.switch_binary,
        value = 0xFF
    })

    tests.assert_called( zwave.request_value, "no value requested", {} )
end

-- channel 1 switch updated
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = 1,
        class_id = class_ids.switch_binary,
        value = 0xFF
    })

    tests.assert_called( zwave.request_value, "0 channel requested", { { "9000", class_ids.switch_binary, 0 } } )
end

-- channel 1 switch updated
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = 2,
        class_id = class_ids.switch_binary,
        value = 0xFF
    })

    tests.assert_called( zwave.request_value, "0 channel requested", { { "9000", class_ids.switch_binary, 0 } } )
end
