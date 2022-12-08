local tests = require("tests")
local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/direct_associations_fixture")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local function test_impl(params)
    fixture.init()
    fixture.add_node(params)
    fixture.check_settings_not_added()
    fixture.remove_node()
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {},
            description = "test with no associations CC"
        },
        {
            value = {
                {
                    association_groups = {
                        {
                            commands = {},
                            group_id = 1,
                            group_name = "Lifeline",
                            profile = {
                                name = "General:Lifeline",
                                profile_lsb = 1,
                                profile_msb = 0
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = class_ids.association_group_info,
                    version = 3
                },
            },
            description = "test with only AGI CC"
        },
        {
            value = {
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
            description = "test with only Associations CC"
        },
        {
            value = {
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
            description = "test with only Multi Channel Associations CC"
        },
        {
            value = {
                {
                    association_groups = {
                        {
                            commands = {},
                            group_id = 1,
                            group_name = "Lifeline",
                            profile = {
                                name = "General:Lifeline",
                                profile_lsb = 1,
                                profile_msb = 0
                            }
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
            },
            description = "test with no association max nodes count"
        },
    }
)
