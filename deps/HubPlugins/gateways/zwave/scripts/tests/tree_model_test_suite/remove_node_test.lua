local test = require("tests")

local tree_model = hubplugins.object("tree_model", "HUB:zwave/scripts/utils")
tree_model:call("insert_child", nil, nil, "")

local child_a = tree_model:call("insert_child", tree_model:call("get_root_node"), "a", "A")
local child_b = tree_model:call("insert_child", tree_model:call("get_root_node"), "b", "B")

local child_ac = tree_model:call("insert_child", child_a, "c", "AC")
local child_acd = tree_model:call("insert_child", child_ac, "d", "ACD")

tree_model:call("remove_node", child_a)

test.assert_eq(tree_model:call("get_id", child_a), "a", "check id of removed node")
test.assert_eq(tree_model:call("get_data", child_a), "A", "check data of removed node")
test.assert_eq(tree_model:call("get_parent", child_a), nil, "check parent of removed node")

test.assert_eq(tree_model:call("get_child", tree_model:call("get_root_node"), "a"), nil, "check child is removed from parent")
test.assert_eq(tree_model:call("get_child", tree_model:call("get_root_node"), "b"), child_b, "check neighbor is present yet")

tree_model:call("remove_node", tree_model:call("get_root_node"))
test.assert_eq(tree_model:call("get_root_node"), nil, "root node must be nil after removing this node")
