local test = require("tests")

local tree_model = hubplugins.object("tree_model", "HUB:zwave/scripts/utils")

tree_model:call("reverse_walk", function(node)
    error("empty tree")
end)

--[[
           r
          / \
    c -- a   b
   / \
  d   e
]]

tree_model:call("insert_child", nil, nil, "r")

local child_a = tree_model:call("insert_child", tree_model:call("get_root_node"), "a", "A")
local child_b = tree_model:call("insert_child", tree_model:call("get_root_node"), "b", "B")
local child_ac = tree_model:call("insert_child", child_a, "c", "C")
local child_acd = tree_model:call("insert_child", child_ac, "d", "D")
local child_ace = tree_model:call("insert_child", child_ac, "e", "E")

local nodes_map = {}
local nodes_count = 0

tree_model:call("reverse_walk", function(node)
    test.assert_eq(nodes_map[tree_model:call("get_data", node)], nil, "no multiple handling")
    local parent = tree_model:call("get_parent", node)

    if parent then
        test.assert_eq(nodes_map[tree_model:call("get_data", parent)], nil, "parent shouldn't be handled before children if reverse walking")
    end

    nodes_map[tree_model:call("get_data", node)] = node
    nodes_count = nodes_count + 1
end)

test.assert_eq(nodes_count, 6, "check number of handled nodes during direct walking")



-- remove current node ( node a ) when walking

nodes_count = 0
tree_model:call("reverse_walk", function(node)
    test.assert_eq(node.children and next(node.children), nil, "check no children on reverse removing")

    tree_model:call("remove_node", node)
    nodes_count = nodes_count + 1
    test.assert_eq(nodes_map[tree_model:call("get_data", node)], node, "check handling of each node during walking")
end)

test.assert_eq(nodes_count, 6, "all nodes are handled even removing nodes during walking")
test.assert_eq(tree_model:call("get_root_node"), nil, "root node is removed")
