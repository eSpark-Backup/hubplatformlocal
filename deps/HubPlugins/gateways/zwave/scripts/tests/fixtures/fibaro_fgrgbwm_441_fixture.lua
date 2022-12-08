local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()

function fixture:create_node(node_id, region)
    return {
        basic_type = 4,
        channels = {
            {
                classes = {
                    {
                        id = 114,
                        version = 1
                    },
                    {
                        firmware_versions = {
                            "25.25"
                        },
                        hardware_version = 0,
                        id = 134,
                        library_type = "enhanced_slave",
                        version = 1,
                        zwave_protocol_version = "3.52"
                    },
                    {
                        id = 38,
                        version = 1
                    },
                    {
                        id = 96,
                        version = 3
                    },
                    {
                        id = 112,
                        parameters = {},
                        version = 1
                    },
                    {
                        id = 50,
                        reset = false,
                        scales = {
                            "kilo_watt_hour",
                            "watt"
                        },
                        type = "electric",
                        version = 2
                    },
                    {
                        id = 49,
                        sensors = {
                            {
                                scales = {
                                    "watt"
                                },
                                sensor_type = "power"
                            }
                        },
                        version = 2
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes = {
                                    1
                                }
                            },
                            {
                                group_id = 5,
                                group_nodes = {
                                    1
                                }
                            }
                        },
                        association_groups_count = 5,
                        id = 133,
                        version = 2
                    },
                    {
                        component_ids = {
                            "wwhite",
                            "red",
                            "green",
                            "blue"
                        },
                        id = 51,
                        version = 1
                    }
                },
                id = 0
            },
            {
                classes = {
                    {
                        id = 38,
                        version = 1
                    }
                },
                id = 1
            },
            {
                classes = {
                    {
                        id = 38,
                        version = 1
                    }
                },
                id = 2
            },
            {
                classes = {
                    {
                        id = 38,
                        version = 1
                    }
                },
                id = 3
            },
            {
                classes = {
                    {
                        id = 38,
                        version = 1
                    }
                },
                id = 4
            },
            {
                classes = {
                    {
                        id = 38,
                        version = 1
                    }
                },
                id = 5
            }
        },
        generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
        id = tostring(node_id),
        is_battery_powered = false,
        is_reachable = false,
        is_smart_include = false,
        manufacturer_id = 271,
        product_id = region == "us" and 8192 or 4096,
        product_type = 2304,
        security = {
            "none"
        },
        security_class_started = {
            "none"
        },
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL"
    }
end


return fixture
