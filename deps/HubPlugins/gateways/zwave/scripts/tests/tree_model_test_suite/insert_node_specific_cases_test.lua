local test = require("tests")

local tree_model = hubplugins.object("tree_model", "HUB:zwave/scripts/utils")

local function data_merge_function(source_data, dest_data)
    dest_data.text = dest_data.text .. source_data.text
end

test.assert_eq(pcall(tree_model.call, tree_model, "insert_node", nil, nil), false, "insert nil node with nil parent to empty tree")

local root = assert(tree_model:call("insert_child", nil, "id", { text = "1" }, data_merge_function), "insert child without parent to empty tree")

test.assert_eq(tree_model:call("get_root_node"), root, "check root")

test.assert_eq(tree_model:call("insert_child", nil, "id", { text = "+1" }, data_merge_function), root, "insert child without patent")
test.assert_eq(tree_model:call("get_data", root).text, "1+1", "check data")

test.assert_eq(pcall(tree_model.call, tree_model, "insert_node", root, nil), false, "insert nil node")
