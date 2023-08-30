local extensions_manager = require "extensions_manager"
local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

if zwave.clear_device_specific then
    zwave.clear_device_specific()
end

if not extensions_manager then
    print("Extensions manager functionality not exists")
    return
end

if not extensions_manager.get_plugin_extensions or not extensions_manager.get_plugin_extension_data then
    print("Extensions api not supported")
    return
end

local extensions = extensions_manager.get_plugin_extensions("zwave")
if not extensions or not next(extensions) then
    print("No extensions for current plugin")
    return
end

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/extensions/device_settings/process_extention_settings")(extensions_manager, extensions)
