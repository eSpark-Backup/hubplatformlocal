local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

-- static variables
fixture.node_id = 9000
fixture.node_id_str = "9000"

function fixture:create_node()
    return {
        basic_type = 4,
        channels = {
            {
                classes = {
                    {
                        id = 32,
                        is_supported = true,
                        version = 1
                    },
                    {
                        id = 112,
                        parameters = {},
                        version = 0
                    },
                    {
                        id = 113,
                        notifications = {
                            {
                                event_types = {
                                    "intrusion"
                                },
                                type = "home_security",
                                unknown_event_types = {}
                            }
                        },
                        v1_supported = false,
                        version = 2
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes = {
                                    1
                                }
                            }
                        },
                        association_groups_count = 1,
                        id = 133,
                        version = 1
                    },
                    {
                        id = 128,
                        version = 1
                    },
                    {
                        id = 114,
                        version = 1
                    },
                    {
                        id = 48,
                        sensor_types = {},
                        version = 1
                    },
                    {
                        firmware_versions = {
                            "4.84"
                        },
                        hardware_version = 0,
                        id = 134,
                        library_type = "routing_slave",
                        version = 1,
                        zwave_protocol_version = "3.52"
                    },
                    {
                        id = 132,
                        version = 2
                    }
                },
                id = 0
            }
        },
        generic_type = "GENERIC_TYPE_SENSOR_BINARY",
        id = fixture.node_id_str,
        is_battery_powered = true,
        is_reachable = false,
        is_smart_include = false,
        manufacturer_id = 265,
        product_id = 258,
        product_type = 8193,
        security = {
            "none"
        },
        security_class_started = {
            "none"
        },
        specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
        wake_up_interval = 5400
    }
end


return fixture
