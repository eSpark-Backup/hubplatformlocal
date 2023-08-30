local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local setting_id = params.setting_id
local value = params.value

assert(setting_id)
print("Set gateway setting value, setting_id: " .. setting_id)

local gateway_monitoring_handler = hubplugins.object( "monitoring", "HUB:zwave/scripts/model/gateway" )
gateway_monitoring_handler:call("set_setting_value", setting_id, value)
