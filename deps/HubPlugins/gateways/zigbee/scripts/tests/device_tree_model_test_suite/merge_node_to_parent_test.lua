local test = require("tests")

local tree_model = hubplugins.object("device_tree_model", "HUB:zigbee/scripts/devices", { text = "R" })

local function data_merge_function(source_data, dest_data)
    dest_data.info.text = dest_data.info.text .. source_data.info.text
end

local root = tree_model:call("insert_child")
local child_a = tree_model:call("insert_child", root, "a", { text = "A" })
local child_b = tree_model:call("insert_child", root, "b", { text = "B" })
local child_ab = tree_model:call("insert_child", child_a, "b", { text = "+B" })

--[[ Actual tree
          r
         / \
   b -- a   b
]]

-- merge root to parent: no changes expected
tree_model:call("merge_node_to_parent", root, data_merge_function)

test.assert_eq(tree_model:call("get_root_node"), root, "root hasn't been changed")
test.assert_eq(tree_model:call("get_child", root, "a"), child_a, "child hasn't been changed")
test.assert_eq(tree_model:call("get_data", root).info.text, "R", "root data hasn't been changed")
test.assert_eq(tree_model:call("get_data", child_a).info.text, "A", "child data hasn't been changed")

-- merge child to parent (root)
tree_model:call("merge_node_to_parent", child_a, data_merge_function)

--[[ Actual tree
          r
           \
            b
]]

test.assert_eq(tree_model:call("get_root_node"), root, "root hasn't been changed")
test.assert_eq(tree_model:call("get_child", root, "a"), nil, "child is merged to root")
test.assert_eq(tree_model:call("get_data", root).info.text, "RA", "data is merged to root")
test.assert_eq(tree_model:call("get_parent", child_a), nil, "child must be removed from tree")
test.assert_eq(tree_model:call("get_child", root, "b"), child_b, "not merged node shouldn't be changed")
test.assert_eq(tree_model:call("get_child", child_a, "b"), nil, "child of merged node must be absent")
test.assert_eq(tree_model:call("get_data", child_b).info.text, "B+B", "data of merged children must be merged according to common rule")