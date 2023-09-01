local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

function fixture:create_node(node_id)
    return {
        basic_type = 4,
        channels = {
            {
                classes = {
                    {
                        id = class_ids.switch_binary,
                        version = 1
                    },
                    {
                        id = class_ids.meter,
                        reset = false,
                        scales = {
                            "kilo_watt_hour",
                            "watt"
                        },
                        type = "electric",
                        version = 2
                    },
                    {
                        id = class_ids.configuration,
                        parameters = {},
                        version = 1
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes = { 1 }
                            }
                        },
                        association_groups_count = 1,
                        id = 133,
                        version = 1
                    },
                    {
                        id = 114,
                        version = 2
                    },
                    {
                        firmware_versions = { "1.1" },
                        hardware_version = 0,
                        id = 134,
                        library_type = "enhanced_slave",
                        version = 1,
                        zwave_protocol_version = "3.67"
                    },
                    {
                        id = 96,
                        version = 3
                    }
                },
                id = 0
            },
            {
                classes = {
                    {
                        id = class_ids.meter,
                        reset = false,
                        scales = {
                            "kilo_watt_hour",
                            "watt"
                        },
                        type = "electric",
                        version = 2
                    },
                    {
                        id = class_ids.switch_binary,
                        version = 1
                    },
                    {
                        id = class_ids.basic,
                        version = 1
                    }
                },
                id = 1
            },
            {
                classes = {
                    {
                        id = class_ids.meter,
                        reset = false,
                        scales = {
                            "kilo_watt_hour",
                            "watt"
                        },
                        type = "electric",
                        version = 2
                    },
                    {
                        id = class_ids.switch_binary,
                        version = 1
                    },
                    {
                        id = class_ids.basic,
                        version = 1
                    }
                },
                id = 2
            },
            {
                classes = {
                    {
                        id = class_ids.meter,
                        reset = false,
                        scales = {
                            "kilo_watt_hour",
                            "watt"
                        },
                        type = "electric",
                        version = 2
                    },
                    {
                        id = class_ids.switch_binary,
                        version = 1
                    },
                    {
                        id = class_ids.basic,
                        version = 1
                    }
                },
                id = 3
            },
            {
                classes = {
                    {
                        id = class_ids.meter,
                        reset = false,
                        scales = {
                            "kilo_watt_hour",
                            "watt"
                        },
                        type = "electric",
                        version = 2
                    },
                    {
                        id = class_ids.switch_binary,
                        version = 1
                    },
                    {
                        id = class_ids.basic,
                        version = 1
                    }
                },
                id = 4
            } 
        },
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",
        id = node_id,
        is_battery_powered = false,
        is_reachable = false,
        is_smart_include = false,
        manufacturer_id = 134,
        product_id = 11,
        product_type = 3,
        security = { "none" },
        security_class_started = { "none" },
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
    }
end


return fixture
