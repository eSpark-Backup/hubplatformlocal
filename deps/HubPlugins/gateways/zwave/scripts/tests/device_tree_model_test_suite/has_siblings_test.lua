local test = require("tests")

local tree_model = hubplugins.object("device_tree_model", "HUB:zwave/scripts/model/devices", { })

local root = assert(tree_model:call("insert_child"))
local child_1 = tree_model:call("insert_child", root, 1, { })

test.assert_eq(tree_model:call("has_siblings", root), false, "root doesn't have siblings")
test.assert_eq(tree_model:call("has_siblings", child_1), false, "check child without siblings")

local child_2 = tree_model:call("insert_child", root, 2, { })

test.assert_eq(tree_model:call("has_siblings", root), false, "root doesn't have siblings")
test.assert_eq(tree_model:call("has_siblings", child_1), true, "check child 1 with siblings")
test.assert_eq(tree_model:call("has_siblings", child_2), true, "check child 2 with siblings")
