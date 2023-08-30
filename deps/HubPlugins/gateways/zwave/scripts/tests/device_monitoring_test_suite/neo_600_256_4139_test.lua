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
                    version = 2
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
                    id = 159,
                    version = 1
                },
                {
                    application_build_number = 52445,
                    application_framework_api_build_number = 52445,
                    application_framework_api_version = "4.1.1",
                    application_version = "4.1.1",
                    firmware_versions = {
                        "2.17"
                    },
                    hardware_version = 16,
                    host_interface_build_number = 0,
                    id = 134,
                    library_type = "routing_slave",
                    protocol_build_number = 91,
                    protocol_version = "6.4.0",
                    sdk_version = "6.81.1",
                    version = 3,
                    zwave_protocol_version = "6.4"
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 96,
                    version = 4
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
                    association_groups_count = 4,
                    id = 142,
                    version = 3
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 37,
                    version = 1
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                }
            },
            id = 2
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 2
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 37,
                    version = 1
                }
            },
            id = 3
        }
    },
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",
    icon_type = "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_THREE_BUTTONS",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 600,
    product_id = 4139,
    product_type = 256,
    security = {
        "none"
    },
    security_class_started = {
        "unauthenticated"
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

-- channel 2 switch updated
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

-- channel 3 switch updated
do
    events_handler({
        event = "value_updated",
        node_id = "9000",
        channel_id = 3,
        class_id = class_ids.switch_binary,
        value = 0xFF
    })

    tests.assert_called( zwave.request_value, "0 channel requested", { { "9000", class_ids.switch_binary, 0 } } )
end
