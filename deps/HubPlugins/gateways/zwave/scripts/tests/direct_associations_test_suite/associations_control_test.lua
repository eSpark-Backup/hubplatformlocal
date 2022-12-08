local tests = require("tests")
local zwave = require("zwave")
local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/direct_associations_fixture")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local set_setting_dictionary_value = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/set_setting_dictionary_value" ) )

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

local function set_setting_value(key, value)
    set_setting_dictionary_value(
        {
            setting_id = assert(fixture.find_associations_setting()).id,
            key = key,
            value = value
        }
    )
end

local function check_associations_setting_value(key, value)
    local setting = assert(fixture.find_associations_setting())
    tests.assert_eq(setting.value[key], value)
end

-- allow some node id's to use in associations
local allowed_nodes_to_use = {}
zwave.has_node = function(node_id)
    for _, allowed_node_id in ipairs(allowed_nodes_to_use) do
        if allowed_node_id == node_id then return true end
    end
    return false
end


-- main


do -- test use unknown association
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1", { {targetNode = 2} })

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        {}
    )

    tests.assert_called(
        zwave.add_node_to_association_group,
        "check associations adding",
        {}
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

do -- test add too much groups
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_2", { {targetNode = 2}, {targetNode = 3}, {targetNode = 4} })

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        {}
    )

    tests.assert_called(
        zwave.add_node_to_association_group,
        "check associations adding",
        {}
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        {}
    )
end

do -- test no target channel
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_2", { {targetNode = 2} })

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        {}
    )

    tests.assert_called(
        zwave.add_node_to_association_group,
        "check associations adding",
        { {"9000", 0, 2, 2} }
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 2} }
    )
end

do -- test with target channel
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_2", { {targetNode = 2, targetChannel = 1} })

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        {}
    )

    tests.assert_called(
        zwave.add_node_to_association_group,
        "check associations adding",
        { {"9000", 0, 2, 2, 1} }
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 2} }
    )
end

do -- test several nodes
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1", { {targetNode = 2, targetChannel = 1}, {targetNode = 3}, {targetNode = 1} })

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        {}
    )

    tests.assert_called_unordered(
        zwave.add_node_to_association_group,
        "check associations adding",
        { {"9000", 0, 1, 2, 1}, {"9000", 0, 1, 3}, {"9000", 0, 1, 1} }
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

send_associations_updated(0, 1,
    {
        {endpoint_id = 1, node_id = 2},
        {node_id = 3},
        {node_id = 1},
    }
)

do -- test all nodes exists
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1", { {targetNode = 2, targetChannel = 1}, {targetNode = 3}, {targetNode = 1} })

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        {}
    )

    tests.assert_called_unordered(
        zwave.add_node_to_association_group,
        "check associations adding",
        {}
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

do -- test delete single node
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1", { {targetNode = 2, targetChannel = 1}, {targetNode = 1} })

    tests.assert_called_unordered(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        { {"9000", 0, 1, 3} }
    )

    tests.assert_called_unordered(
        zwave.add_node_to_association_group,
        "check associations adding",
        {}
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

do -- test delete several
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1", { {targetNode = 1} })

    tests.assert_called_unordered(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        { {"9000", 0, 1, 2, 1}, {"9000", 0, 1, 3} }
    )

    tests.assert_called_unordered(
        zwave.add_node_to_association_group,
        "check associations adding",
        {}
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

do -- test delete all
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1")

    tests.assert_called(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        { {"9000", 0, 1, 2, 1}, {"9000", 0, 1, 3}, {"9000", 0, 1, 1} }
    )

    tests.assert_called_unordered(
        zwave.add_node_to_association_group,
        "check associations adding",
        {}
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

do -- test delete and add
    allowed_nodes_to_use = {2, 3, 4}
    zwave.remove_node_from_association_group = tests.create_mock()
    zwave.add_node_to_association_group = tests.create_mock()
    zwave.request_association_group = tests.create_mock()

    set_setting_value("0_1", { {targetNode = 2, targetChannel = 1}, {targetNode = 4} })

    tests.assert_called_unordered(
        zwave.remove_node_from_association_group,
        "check associations deletion",
        { {"9000", 0, 1, 3}, {"9000", 0, 1, 1} }
    )

    tests.assert_called_unordered(
        zwave.add_node_to_association_group,
        "check associations adding",
        { {"9000", 0, 1, 4} }
    )

    tests.assert_called(
        zwave.request_association_group,
        "check associations request",
        { {"9000", 0, 1} }
    )
end

fixture.remove_node()
