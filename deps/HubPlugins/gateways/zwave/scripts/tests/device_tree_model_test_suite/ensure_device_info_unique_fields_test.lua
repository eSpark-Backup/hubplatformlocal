local tests = require("tests")


local initial_data = { test_table = { } }
local tree_model = hubplugins.object("device_tree_model", "HUB:zwave/scripts/model/devices", initial_data)

local root = tree_model:call("insert_child")
local child = tree_model:call("insert_child", root, "child")

local root_info = tree_model:call("ensure_device_info_unique_fields", root, "test_table")
root_info.test_table.b = "b"

local child_info = tree_model:call("ensure_device_info_unique_fields", child, "test_table")
child_info.test_table.c = "c"

initial_data.test_table.a = "a"

local root_test_table = tree_model:call("get_data", root).info.test_table
tests.assert_ne( root_test_table, initial_data.test_table, "new root test table must be created" )
tests.assert_eq( root_test_table.a, "a", "check initial root data" )
tests.assert_eq( root_test_table.b, "b", "check added root data" )
tests.assert_eq( root_test_table.c, nil, "check no added child data in root" )

local child_test_table = tree_model:call("get_data", child).info.test_table
tests.assert_ne( child_test_table, initial_data.test_table, "new child test table must be created" )
tests.assert_eq( child_test_table.a, "a", "check initial child data" )
tests.assert_eq( child_test_table.c, "c", "check added child data" )
tests.assert_eq( child_test_table.b, nil, "check no added root data in child" )

tests.assert_eq( root_test_table, 
    tree_model:call("ensure_device_info_unique_fields", root, "test_table").test_table, 
    "root test table must be the same after repeated ensuring unique fields" )
tests.assert_eq( child_test_table, 
    tree_model:call("ensure_device_info_unique_fields", child, "test_table").test_table, 
    "child test table must be the same after repeated ensuring unique fields" )
