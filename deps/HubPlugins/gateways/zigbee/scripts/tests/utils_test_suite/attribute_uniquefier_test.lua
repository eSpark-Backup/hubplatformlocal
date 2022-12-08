local tests = require("tests")

local attr_uniquefier = loadfile("HUB:zigbee/scripts/utils/attribute_uniquefier")()

tests.assert_eq(attr_uniquefier.unique(1, 1, "cluster1", "attr_1"), true)
tests.assert_eq(attr_uniquefier.unique(1, 1, "cluster1", "attr_1"), false)

tests.assert_eq(attr_uniquefier.unique_by_ids(1, 1, 1, 1), true)
tests.assert_eq(attr_uniquefier.unique_by_ids(1, 1, 1, 1), false)
tests.assert_eq(attr_uniquefier.unique(1, 1, "1", "1"), false)
tests.assert_eq(attr_uniquefier.unique_by_ids(1, 1, 1.0, 1.0), false)
