local test = require("tests")

local base_device_info = { name = "my device" }

local tree_model = hubplugins.object("device_tree_model", "HUB:zwave/scripts/model/devices", base_device_info)

local root = assert(tree_model:call("insert_child"))
local root_data = assert(tree_model:call("get_data", root))

test.assert_eq(tree_model:call("get_base_device_name"), "my device", "check base device name")

test.assert_eq(root_data.info.name, "my device", "check root data")
test.assert_ne(root_data.items, nil, "check root items")
test.assert_eq(next(root_data.items), nil, "check root items empty")

local child = tree_model:call("insert_child", root, 0, { name = "your device", device_id = "child" })
local child_data = assert(tree_model:call("get_data", child))
test.assert_eq(child_data.info.name, "your device", "check child data")
test.assert_ne(child_data.items, nil, "check child items")
test.assert_eq(next(child_data.items), nil, "check child items empty")
