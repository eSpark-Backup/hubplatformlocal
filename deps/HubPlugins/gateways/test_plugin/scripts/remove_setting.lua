local params = ... or {}

local core = require "core"

print(string.format("TEST_PLUGIN: remove setting, setting_id: %s", params.setting_id))
core.remove_setting(params.setting_id)
