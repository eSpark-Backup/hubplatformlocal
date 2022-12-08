local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()


function fixture:create_node(node_id)
    return {
        id  = node_id,
        basic_type = 4,
        channels = {
            {
                classes  = {
                    {
                        id = 94,
                        version  = 2
                    },
                    {
                        id = 38,
                        version = 2
                    },
                    {
                        component_ids = {
                            "red",
                            "green",
                            "blue"
                        },
                        id = 51,
                        version = 1
                    },
                    {
                        id = 50,
                        reset = 1,
                        scales = {
                            0,
                            2,
                            4,
                            5
                        },
                        type = 1,
                        version = 3
                    },
                    {
                        id = 129,
                        version  = 1
                    },
                    {
                        id = 96,
                        version  = 4
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
                        association_groups_count = 2,
                        id = 142,
                        version  = 3
                    },
                    {
                        id = 112,
                        version = 1
                    },
                    {
                        association_groups_count = 0,
                        id = 133,
                        version = 2
                    },
                    {
                        association_groups = {
                            {
                                commands = {
                                    {
                                        class_id = 32,
                                        command_id = 3
                                    },
                                    {
                                        class_id = 130,
                                        command_id = 1
                                    }
                                },
                                group_id = 1,
                                group_name = "Lifeline",
                                profile_lsb = 0,
                                profile_msb = 0
                            },
                            {
                                commands = {
                                    {
                                        class_id = 37,
                                        command_id = 1
                                    },
                                    {
                                        class_id = 32,
                                        command_id = 1
                                    },
                                    {
                                        class_id = 38,
                                        command_id = 4
                                    },
                                    {
                                        class_id = 38,
                                        command_id = 5
                                    },
                                    {
                                        class_id = 38,
                                        command_id = 1
                                    }
                                },
                                group_id = 2,
                                group_name = "Retransmit",
                                profile_lsb = 0,
                                profile_msb = 0
                            }
                        },
                        association_groups_count = 2,
                        id = 89,
                        version = 1
                    },
                    {
                        id = 114,
                        version = 2
                    },
                    {
                        id = 134,
                        version = 2
                    }
                },
                id = 0
            },
            {
                classes = {
                    {
                        id = 94,
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
                        id = 38,
                        version = 2
                    },
                    {
                        association_groups_count = 0,
                        id = 142,
                        version = 1
                    },
                    {
                        id = 50,
                        reset = 1,
                        scales = {
                            0,
                            2,
                            4,
                            5
                        },
                        type = 1,
                        version = 3
                    },
                    {
                        id = 129,
                        version = 1
                    },
                    {
                        id = 112,
                        version = 1
                    },
                    {
                        id = 134,
                        version = 1
                    }
                },
                id = 1
            },
            {
                classes = {
                    {
                        id = 94,
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
                        id = 38,
                        version = 2
                    },
                    {
                        component_ids = {

                        },
                        id = 51,
                        version = 1
                    }
                },
                id = 2
            }
        },
        generic_type = "GENERIC_TYPE_SWITCH_MULTILEVEL",
        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL",
        icon_type = "ICON_TYPE_GENERIC_LIGHT_DIMMER_SWITCH",
        is_battery_powered = false,
        is_reachable = false,
        manufacturer_id = 134,
        product_id = 99,
        product_type = 259,
        security = { "none" },
        security_class_started = { "none" },
    }
end


return fixture