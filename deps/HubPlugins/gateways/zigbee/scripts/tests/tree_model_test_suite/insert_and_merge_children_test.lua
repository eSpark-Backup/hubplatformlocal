local test = require("tests")

local root_data = { text = "root_data" }

local tree_model = hubplugins.object("tree_model", "HUB:zigbee/scripts/utils")

local function data_merge_function(source_data, dest_data)
    dest_data.text = dest_data.text .. source_data.text
end

local root = tree_model:call("insert_child", nil, nil, root_data)
local child_a = tree_model:call("insert_child", root, "a", { text = "A" }, data_merge_function)
local child_b = tree_model:call("insert_child", root, "b", { text = "B" }, data_merge_function)

--[[ Actual tree
                r
               / \
              a   b
]]

test.assert_eq(tree_model:call("insert_child", root, "a", { text = "+A" }, data_merge_function), child_a, "insert child with existing id")
test.assert_eq(tree_model:call("get_data", child_a).text, "A+A", "check merged data")
test.assert_eq(tree_model:call("get_data", child_b).text, "B", "check nont merged node data")

local child_ac = tree_model:call("insert_child", child_a, "c", { text = "C" }, data_merge_function)
local child_aa = tree_model:call("insert_child", child_a, "a", { text = "A" }, data_merge_function)
local child_aae = tree_model:call("insert_child", child_aa, "e", { text = "E" }, data_merge_function)
local child_aax = tree_model:call("insert_child", child_aa, "x", { text = "X" }, data_merge_function)

local child_ba = tree_model:call("insert_child", child_b, "a", { text = "-A" }, data_merge_function)
local child_bac = tree_model:call("insert_child", child_ba, "c", { text = "-C" }, data_merge_function)
local child_baa = tree_model:call("insert_child", child_ba, "a", { text = "-A" }, data_merge_function)
local child_baae = tree_model:call("insert_child", child_baa, "e", { text = "-E" }, data_merge_function)
local child_baay = tree_model:call("insert_child", child_baa, "y", { text = "-Y" }, data_merge_function)

--[[ Actual tree
                r
               / \
    e -- a -- a   b
        /    /     \
       x    c       a -- a -- e
                   /    /
                  c    y
]]

-- insert (merge) child_ba -> child_a
tree_model:call("insert_node", root, child_ba, data_merge_function)
--[[ Actual tree
        y        r
         \      / \
     e -- a -- a   b
         /    /
        x    c
]]

test.assert_eq(tree_model:call("get_child", child_b, "a"), nil, "moved node shouldn't exist in previous parent")
test.assert_eq(tree_model:call("get_parent", child_ba), nil, "parent must be nil if node removed (merged)")
test.assert_eq(next(child_ba.children), nil, "children must be empty if node merged (removed)")

local function check_merged_child(parent, child_id, child, data_text)
    test.assert_eq(tree_model:call("get_child", parent, child_id), child, "check child " .. child_id)
    test.assert_eq(tree_model:call("get_id", child), child_id, "check child id")
    test.assert_eq(tree_model:call("get_parent", child), parent)
    test.assert_eq(tree_model:call("get_data", child).text, data_text, "check child data")
end

check_merged_child(root, "a", child_a, "A+A-A")
check_merged_child(child_a, "c", child_ac, "C-C")
check_merged_child(child_a, "a", child_aa, "A-A")
check_merged_child(child_aa, "e", child_aae, "E-E")
check_merged_child(child_aa, "x", child_aax, "X")
check_merged_child(child_aa, "y", child_baay, "-Y")


-- merge child_a with root
tree_model:call("insert_node", nil, child_a, data_merge_function)
--[[ Actual tree
                 r -- c
                / \
          x -- a   b
              / \
             y   e
]]

test.assert_eq(tree_model:call("get_data", root).text, "root_dataA+A-A", "check root data after merge to root")
test.assert_eq(tree_model:call("get_parent", child_a), nil, "parent must be nil if node removed (merged)")
test.assert_eq(next(child_a.children), nil, "children must be empty if node merged (removed)")

check_merged_child(root, "a", child_aa, "A-A")
check_merged_child(root, "b", child_b, "B")
check_merged_child(root, "c", child_ac, "C-C")


-- insert nodes to place where they are. Tree shouldn't be changed
tree_model:call("insert_node", root, child_aa, data_merge_function)
tree_model:call("insert_node", nil, root, data_merge_function)
--[[ Actual tree
                 r -- c
                / \
          x -- a   b
              / \
             y   e
]]

test.assert_eq(tree_model:call("get_data", root).text, "root_dataA+A-A", "check root data after fake merge")
check_merged_child(root, "a", child_aa, "A-A")
check_merged_child(child_aa, "x", child_aax, "X")
