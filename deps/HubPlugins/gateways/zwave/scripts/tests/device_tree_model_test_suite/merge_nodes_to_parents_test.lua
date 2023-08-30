local test = require("tests")

local tree_model = hubplugins.object("device_tree_model", "HUB:zwave/scripts/model/devices", { text = "R" })

local function data_merge_function(source_data, dest_data)
    dest_data.info.text = dest_data.info.text .. source_data.info.text
end

local root = tree_model:call("insert_child")
local child_a = tree_model:call("insert_child", root, "a", { text = "A" })
local child_b = tree_model:call("insert_child", root, "b", { text = "B" })
local child_ab = tree_model:call("insert_child", child_a, "b", { text = "B" })
local child_bc = tree_model:call("insert_child", child_b, "c", { text = "C" })
local child_bcd = tree_model:call("insert_child", child_bc, "d", { text = "D" })


--[[ Actual tree
          r
         / \
   b -- a   b -- c -- d
]]

-- merge nodes to parents by predicate
tree_model:call("merge_nodes_to_parents", data_merge_function, function(node)
    return node.id == "b"
end)

--[[ Actual tree
          r
         / \
        a   c -- d
]]


test.assert_eq(tree_model:call("get_root_node"), root, "root hasn't been changed")
test.assert_eq(tree_model:call("get_child", root, "a"), child_a, "child hasn't been changed")
test.assert_eq(tree_model:call("get_child", root, "c"), child_bc, "grand child has become child because child has been merged")
test.assert_eq(tree_model:call("get_child", child_bc, "d"), child_bcd, "grand-grand child shouldn't be changed")
test.assert_eq(tree_model:call("get_data", root).info.text, "RB", "root data has been merged with 'b' child")
test.assert_eq(tree_model:call("get_data", child_a).info.text, "AB", "child 'a' data has been merged with 'b' child")

-- merge nodes to parents without predicate
tree_model:call("merge_nodes_to_parents", data_merge_function)

--[[ Actual tree
          r
]]

test.assert_eq(tree_model:call("get_root_node"), root, "root hasn't been changed")
test.assert_eq(next(root.children), nil, "root children are empty")
test.assert_eq(string.len(tree_model:call("get_data", root).info.text), 6, "all symbols must be merged to root")
