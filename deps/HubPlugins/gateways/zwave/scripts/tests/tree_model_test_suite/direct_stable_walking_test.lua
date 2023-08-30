local test = require("tests")

local tree_model = hubplugins.object("tree_model", "HUB:zwave/scripts/utils")

tree_model:call("reverse_walk", function(node)
    error("empty tree")
end)

--[[
        .--r--.
       /  / \  \
      a  b   c  d
     /|\       /|\ 
    e f g     o p q
]]

tree_model:call("insert_child", nil, nil, "R")

local child_c = tree_model:call("insert_child", tree_model:call("get_root_node"), "c", "C")
local child_a = tree_model:call("insert_child", tree_model:call("get_root_node"), "a", "A")
local child_b = tree_model:call("insert_child", tree_model:call("get_root_node"), "b", "B")
local child_d = tree_model:call("insert_child", tree_model:call("get_root_node"), "d", "D")


local child_ae = tree_model:call("insert_child", child_a, "e", "E")
local child_ag = tree_model:call("insert_child", child_a, "g", "G")
local child_af = tree_model:call("insert_child", child_a, "f", "F")


local child_dq = tree_model:call("insert_child", child_d, "q", "Q")
local child_do = tree_model:call("insert_child", child_d, "o", "O")
local child_dp = tree_model:call("insert_child", child_d, "p", "P") 


local handle_node_data = test.create_mock()

tree_model:call("direct_stable_walk", function(node)
    handle_node_data( tree_model:call("get_data", node) )
end)

test.assert_called(handle_node_data, "check stable walking order", { 
    { "R" },
    { "A" },
    { "E" },
    { "F" },
    { "G" },
    { "B" },
    { "C" },
    { "D" },
    { "O" },
    { "P" },
    { "Q" }
})


