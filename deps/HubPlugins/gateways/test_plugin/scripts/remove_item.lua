local params = ... or {}

local core = require "core"

print("TEST_PLUGIN: Remove item: ", params.item_id)
core.remove_item(params.item_id)
