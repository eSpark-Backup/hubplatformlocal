local tests = require("tests")
local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/direct_associations_fixture")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )

fixture.init()
fixture.add_node(
    {
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
                {
                    commands = {},
                    group_id = 2,
                    group_name = "TestGroup1",
                    profile = {
                        name = "TestProfileName1"
                    },
                    commands = {{name = "Command1"}, {name = "Command2"}}
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
                    group_nodes_max_count = 5
                },
                {
                    group_id = 2,
                    group_nodes = {},
                    group_nodes_max_count = 2
                }
            },
            association_groups_count = 2,
            id = class_ids.multi_channel_associations,
            version = 3
        },
    },
    {
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
                        group_nodes = {},
                        group_nodes_max_count = 1
                    },
                    {
                        group_id = 2,
                        group_nodes = {},
                        group_nodes_max_count = 3
                    }
                },
                association_groups_count = 2,
                id = class_ids.multi_channel_associations,
                version = 3
            },
        }
    }
)

local function send_associations_updated(channel_id, group_id, group_nodes)
    events_handler( {
        event = "association_updated",
        node_id = 9000,
        class_id = 58,
        channel_id = channel_id,
        group_id = group_id,
        group_nodes = group_nodes
    } )
end

local function check_associations_setting_value(key, value)
    local setting = assert(fixture.find_associations_setting())
    tests.assert_eq(setting.value[key], value)
end

do -- test update lifeline group
    send_associations_updated( 0, 1,
        {
            {endpoint_id = 0, node_id = 1},
            {endpoint_id = 1, node_id = 1}
        }
    )

    check_associations_setting_value("0_1",
        {
            {targetNode = 1, targetChannel = 0},
            {targetNode = 1, targetChannel = 1}
        }
    )
end

do -- test remove from lifeline group
    send_associations_updated( 0, 1,
        {
            {endpoint_id = 0, node_id = 1}
        }
    )

    check_associations_setting_value("0_1",
        {
            {targetNode = 1, targetChannel = 0}
        }
    )
end

do -- test update another group from root channel
    send_associations_updated( 0, 2,
        {
            {endpoint_id = 2, node_id = 1},
            {node_id = 2}
        }
    )

    check_associations_setting_value("0_2",
        {
            {targetNode = 1, targetChannel = 2},
            {targetNode = 2}
        }
    )
end

do -- test clear group
    send_associations_updated( 0, 2,
        {}
    )

    check_associations_setting_value("0_2",
        {}
    )
end

do -- test update group grom child channel
    send_associations_updated( 1, 2,
        {
            {endpoint_id = 0, node_id = 2},
            {endpoint_id = 1, node_id = 3},
            {endpoint_id = 2, node_id = 3},
            {node_id = 4},
        }
    )

    check_associations_setting_value("1_2",
        {
            {targetNode = 2, targetChannel = 0},
            {targetNode = 3, targetChannel = 1},
            {targetNode = 3, targetChannel = 2},
            {targetNode = 4},
        }
    )
end

fixture.remove_node()
