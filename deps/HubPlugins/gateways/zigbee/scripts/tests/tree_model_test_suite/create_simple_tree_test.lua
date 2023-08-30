local test = require( "tests" )

local root_data = "root_data"
local child_data = "child_data"

local tree_model = hubplugins.object("tree_model", "HUB:zigbee/scripts/utils")
tree_model:call("insert_child", nil, nil, root_data)

local root = assert(tree_model:call("get_root_node"), "check root node")
local child = assert(tree_model:call("insert_child",  root, 1, child_data ), "insert child")

test.assert_eq(tree_model:call("get_parent", child), root, "check parent of child")
test.assert_eq(root, tree_model:call("get_root_node"), "root hasn't been changed")
test.assert_eq(tree_model:call("get_parent", root), nil, "check parent of root")

test.assert_eq( tree_model:call("get_id", root), nil, "root node id is nil by default" )
test.assert_eq( tree_model:call("get_child", root, 1), child , "getting child by id" )
test.assert_eq( tree_model:call("get_child", root, 0), nil , "getting child by wrong id" )

test.assert_eq( tree_model:call("get_id", child), 1, "check child id" )

test.assert_eq( tree_model:call("get_data",  root ), root_data, "check root data" )
test.assert_eq( tree_model:call("get_data",  child ), child_data, "check child data" )