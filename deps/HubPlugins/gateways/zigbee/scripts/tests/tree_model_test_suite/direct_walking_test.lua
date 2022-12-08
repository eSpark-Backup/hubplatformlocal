

local test = require("tests")

local function test_impl( direct_walk_method_name )
    local tree_model = hubplugins.object("tree_model", "HUB:zigbee/scripts/utils")

    tree_model:call(direct_walk_method_name, function(node)
        error("empty tree")
    end)

    tree_model:call("insert_child", nil, nil, "r")

    --[[
               r
              / \
        c -- a   b
       / \
      d   e
    ]]

    local child_a = tree_model:call("insert_child", tree_model:call("get_root_node"), "a", "A")
    local child_b = tree_model:call("insert_child", tree_model:call("get_root_node"), "b", "B")
    local child_ac = tree_model:call("insert_child", child_a, "c", "C")
    local child_acd = tree_model:call("insert_child", child_ac, "d", "D")
    local child_ace = tree_model:call("insert_child", child_ac, "e", "E")

    local nodes_map = {  }
    local nodes_count = 0

    tree_model:call(direct_walk_method_name, function(node)
        test.assert_eq(nodes_map[tree_model:call("get_data", node)], nil, "no multiple handling")
        local parent = tree_model:call("get_parent", node)

        if parent then
            test.assert_eq(nodes_map[tree_model:call("get_data", parent)], parent, "parent must be handled before children if direct walking")
        end

        nodes_map[tree_model:call("get_data", node)] = node
        nodes_count = nodes_count + 1
    end)

    test.assert_eq(nodes_count, 6, "check number of handled nodes during direct walking")



    -- remove current node (node a) when walking

    nodes_count = 0
    tree_model:call(direct_walk_method_name, function(node)
        if nodes_count ~= 0 then
            test.assert_ne(tree_model:call("get_parent", node), nil, "check nt nil parent on direct removing")
        end

        tree_model:call("remove_node", node)
        nodes_count = nodes_count + 1
        test.assert_eq(nodes_map[tree_model:call("get_data", node)], node, "check handling of each node during walking")
    end)

    test.assert_eq(nodes_count, 6, "all nodes are handled even removing nodes during walking")
end

test.run_with_params( test_impl,
{
    {
        value = "direct_walk",
        description = "direct walk"
    },
    {
        value = "direct_stable_walk",
        description = "direct stable walk"
    }
} )