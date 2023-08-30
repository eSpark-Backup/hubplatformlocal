local params = ...

local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

require("HUB:zwave/scripts/helpers/scripts_preload")

if not hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_device_adding_enabled")() then
    print( "Controller has no permissions to start exclude" )
    local core = assert(require("core"), "Fail to load the 'core' functionality")
    core.send_ui_broadcast({ event = "exclude_finished_error" })
    return
end

if params and params.deviceId then
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local core = assert(require("core"), "Fail to load the core functionality")

    local device_id = core.get_root_device_id(params.deviceId)
    if device_id == nil then
        print("Unable to find device with deviceId=" .. device_id)
        return
    end

    local device = core.get_device(device_id)
    assert(device)
    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_exclusion")(nil, nil, nil, device.device_type_id)
    dev_handler:call("exclude_device", device_id)
else
    zwave.start_exclude()
end
