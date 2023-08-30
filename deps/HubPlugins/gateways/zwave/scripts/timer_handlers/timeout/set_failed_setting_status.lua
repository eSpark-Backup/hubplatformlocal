local params = ...

local core = require "core"

print("set failed setting status by timeout for setting " .. params.setting_id)

core.update_setting_status(params.setting_id, "failed")
