local tests = require("tests")
local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/direct_associations_fixture")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local function test_impl(params)
    fixture.init()
    fixture.add_node(params.command_classes, params.additional_channels_command_classes)
    local setting = fixture.find_associations_groups_setting()
    tests.assert_ne(setting, nil, "Check setting added")

    fixture.check_group_setting_static_fields(setting)
    tests.assert_eq(setting.value, params.setting_value, "Check setting value")

    fixture.remove_node()
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                command_classes = {
                    {
                        association_groups = {
                            {
                                commands = {},
                                group_id = 1,
                                group_name = "Lifeline",
                                profile = {}
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.association_group_info,
                        version = 3
                    },
                    {
                        association_groups = {
                            {
                                group_id = 1,
                                group_nodes = {
                                    {
                                        node_id = 1
                                    }
                                },
                                group_nodes_max_count = 5
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.associations,
                        version = 3
                    },
                },
                setting_value = {
                    ["0_1"] = {
                        name = "Lifeline",
                        channel = 0,
                        maxAssociationsNumber = 5
                    }
                }

            },
            description = "test with associations CC"
        },

        {
            value = {
                command_classes = {
                    {
                        association_groups = {
                            {
                                commands = {},
                                group_id = 1,
                                group_name = "Lifeline",
                                profile = {}
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.association_group_info,
                        version = 3
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
                                },
                                group_nodes_max_count = 5
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.multi_channel_associations,
                        version = 3
                    },
                },
                setting_value = {
                    ["0_1"] = {
                        name = "Lifeline",
                        channel = 0,
                        maxAssociationsNumber = 5
                    }
                }

            },
            description = "test with multi channel associations CC"
        },

        {
            value = {
                command_classes = {
                    {
                        association_groups = {
                            {
                                commands = {},
                                group_id = 1,
                                group_name = "Lifeline",
                                profile = {}
                            },
                            {
                                commands = {},
                                group_id = 2,
                                group_name = "Lifeline2",
                                profile = {}
                            }
                        },
                        association_groups_count = 2,
                        id = class_ids.association_group_info,
                        version = 3
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
                                },
                                group_nodes_max_count = 5
                            },
                            {
                                group_id = 2,
                                group_nodes = {},
                                group_nodes_max_count = 8
                            }
                        },
                        association_groups_count = 2,
                        id = class_ids.multi_channel_associations,
                        version = 3
                    },
                },
                setting_value = {
                    ["0_1"] = {
                        name = "Lifeline",
                        channel = 0,
                        maxAssociationsNumber = 5
                    },
                    ["0_2"] = {
                        name = "Lifeline2",
                        channel = 0,
                        maxAssociationsNumber = 8
                    }
                }

            },
            description = "test with several groups"
        },

        {
            value = {
                command_classes = {
                    {
                        association_groups = {
                            {
                                commands = {},
                                group_id = 1,
                                group_name = "Lifeline",
                                profile = {
                                    name = "TestProfileName"
                                }
                            },
                        },
                        association_groups_count = 1,
                        id = class_ids.association_group_info,
                        version = 3
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
                                },
                                group_nodes_max_count = 5
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.multi_channel_associations,
                        version = 3
                    },
                },
                setting_value = {
                    ["0_1"] = {
                        name = "Lifeline",
                        channel = 0,
                        maxAssociationsNumber = 5,
                        profile = "TestProfileName"
                    }
                }

            },
            description = "test with profile name"
        },

        {
            value = {
                command_classes = {
                    {
                        association_groups = {
                            {
                                commands = {},
                                group_id = 1,
                                group_name = "Lifeline",
                                profile = {
                                    name = "TestProfileName"
                                },
                                commands = {{name = "Command1"}, {name = "Command2"}}
                            },
                        },
                        association_groups_count = 1,
                        id = class_ids.association_group_info,
                        version = 3
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
                                },
                                group_nodes_max_count = 5
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.multi_channel_associations,
                        version = 3
                    },
                },
                setting_value = {
                    ["0_1"] = {
                        name = "Lifeline",
                        channel = 0,
                        maxAssociationsNumber = 5,
                        profile = "TestProfileName",
                        commands = { "Command1", "Command2" }
                    }
                }

            },
            description = "test with commands"
        },

        {
            value = {
                command_classes = {
                    {
                        association_groups = {
                            {
                                commands = {},
                                group_id = 1,
                                group_name = "Lifeline",
                                profile = {
                                    name = "TestProfileName"
                                },
                                commands = {{name = "Command1"}, {name = "Command2"}}
                            },
                        },
                        association_groups_count = 1,
                        id = class_ids.association_group_info,
                        version = 3
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
                                },
                                group_nodes_max_count = 5
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.multi_channel_associations,
                        version = 3
                    },
                },
                additional_channels_command_classes = {
                    {
                        {
                            association_groups = {
                                {
                                    commands = {},
                                    group_id = 1,
                                    group_name = "TestGroup2",
                                    profile = {
                                        name = "TestProfileName2"
                                    },
                                    commands = {{name = "Command2_1"}, {name = "Command2_2"}}
                                },
                                {
                                    commands = {},
                                    group_id = 2,
                                    group_name = "TestGroup3",
                                    profile = {
                                        name = "TestProfileName3"
                                    },
                                    commands = {{name = "Command3_1"}, {name = "Command3_2"}}
                                },
                            },
                            association_groups_count = 2,
                            id = class_ids.association_group_info,
                            version = 3
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
                                    },
                                    group_nodes_max_count = 6
                                },
                                {
                                    group_id = 2,
                                    group_nodes = {
                                        {
                                            endpoint_id = 0,
                                            node_id = 1
                                        }
                                    },
                                    group_nodes_max_count = 7
                                }
                            },
                            association_groups_count = 2,
                            id = class_ids.multi_channel_associations,
                            version = 3
                        },
                    },
                    {
                        {
                            association_groups = {
                                {
                                    commands = {},
                                    group_id = 1,
                                    group_name = "TestGroup4",
                                    profile = {
                                        name = "TestProfileName4"
                                    },
                                    commands = {{name = "Command4_1"}, {name = "Command4_2"}}
                                },
                                {
                                    commands = {},
                                    group_id = 2,
                                    group_name = "TestGroup5",
                                    profile = {
                                        name = "TestProfileName5"
                                    },
                                    commands = {{name = "Command5_1"}, {name = "Command5_2"}}
                                },
                            },
                            association_groups_count = 2,
                            id = class_ids.association_group_info,
                            version = 3
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
                                    },
                                    group_nodes_max_count = 8
                                },
                                {
                                    group_id = 2,
                                    group_nodes = {
                                        {
                                            endpoint_id = 0,
                                            node_id = 1
                                        }
                                    },
                                    group_nodes_max_count = 9
                                }
                            },
                            association_groups_count = 2,
                            id = class_ids.multi_channel_associations,
                            version = 3
                        },
                    }
                },
                setting_value = {
                    ["0_1"] = {
                        name = "Lifeline",
                        channel = 0,
                        maxAssociationsNumber = 5,
                        profile = "TestProfileName",
                        commands = { "Command1", "Command2" }
                    },
                    ["1_1"] = {
                        name = "TestGroup2",
                        channel = 1,
                        maxAssociationsNumber = 6,
                        profile = "TestProfileName2",
                        commands = { "Command2_1", "Command2_2" }
                    },
                    ["1_2"] = {
                        name = "TestGroup3",
                        channel = 1,
                        maxAssociationsNumber = 7,
                        profile = "TestProfileName3",
                        commands = { "Command3_1", "Command3_2" }
                    },
                    ["2_1"] = {
                        name = "TestGroup4",
                        channel = 2,
                        maxAssociationsNumber = 8,
                        profile = "TestProfileName4",
                        commands = { "Command4_1", "Command4_2" }
                    },
                    ["2_2"] = {
                        name = "TestGroup5",
                        channel = 2,
                        maxAssociationsNumber = 9,
                        profile = "TestProfileName5",
                        commands = { "Command5_1", "Command5_2" }
                    }
                }

            },
            description = "test with several channels"
        }
    }
)
