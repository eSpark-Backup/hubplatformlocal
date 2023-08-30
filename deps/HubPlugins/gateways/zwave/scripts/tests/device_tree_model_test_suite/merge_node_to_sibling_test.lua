local test = require("tests")

local tree_model = hubplugins.object("device_tree_model", "HUB:zwave/scripts/model/devices", { text = "R" })

local function data_merge_function(source_data, dest_data)
    dest_data.info.text = dest_data.info.text .. source_data.info.text
end

local root = tree_model:call("insert_child")
local child_a = tree_model:call("insert_child", root, "a", { text = "A" })
local child_b = tree_model:call("insert_child", root, "b", { text = "B" })
local child_c = tree_model:call("insert_child", root, "c", { text = "C" })
local child_ab = tree_model:call("insert_child", child_a, "b", { text = "X" })

--[[ Actual tree
          r - c
         / \
   b -- a   b 
]]

-- merge siblings
tree_model:call("merge_node_to_sibling", root, "a", "b", data_merge_function)

test.assert_eq(tree_model:call("get_root_node"), root, "root hasn't been changed")
test.assert_eq(tree_model:call("get_child", root, "a"), nil, "source child is absent")
test.assert_eq(tree_model:call("get_child", root, "b"), child_b, "destination is not changed")
test.assert_eq(tree_model:call("get_data", child_b).info.text, "BA", "data has been merged")
test.assert_eq(tree_model:call("get_child", child_b, "b"), child_ab, "source child has been moved")
