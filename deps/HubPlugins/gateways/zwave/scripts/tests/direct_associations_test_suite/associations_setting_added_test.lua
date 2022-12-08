local tests = require("tests")
local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/direct_associations_fixture")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local function test_impl(params)
    fixture.init()
    fixture.add_node(params.command_classes, params.additional_channels_command_classes)
    local setting = fixture.find_associations_setting()
    tests.assert_ne(setting, nil, "Check setting added")

    fixture.check_associations_setting_static_fields(setting)
    tests.assert_eq(setting.target_channel_supported, params.target_channel_supported, "Check setting target_channel_supported")

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
                target_channel_supported = false
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
                target_channel_supported = true
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
                            }
                        },
                        association_groups_count = 1,
                        id = class_ids.association_group_info,
                        version = 3
                    },
                    {
                        association_groups = {},
                        association_groups_count = 1,
                        id = class_ids.multi_channel_associations,
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
                target_channel_supported = false
            },
            description = "test with empty multi channel associations CC"
        }
    }
)
