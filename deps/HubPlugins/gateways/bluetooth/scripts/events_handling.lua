local params = ...

local bluetooth = require "bluetooth"
if not bluetooth then
    print("Fail to load bluetooth functionality")
    return
end

local core = require "core"
if not core then
    print("Fail to load core functionality")
    return
end

if params.event == "device_connected" or params.event == "device_paired" then
    local storage = require "storage"
    if not storage then
        print("Fail to load storage functionality")
        return
    end

    local deviceModel = storage.get_string("selectedDeviceModel")
    if not deviceModel then
        print("Invalid device model")
        return
    end

    storage.delete("selectedDeviceModel")

    local handler = require("HUB:bluetooth/scripts/devices/device_handler")
    handler:add_device(params.deviceAddress, deviceModel)
elseif params.event == "device_disconnected" or params.event == "device_unpaired" then
    local handler = require("HUB:bluetooth/scripts/devices/device_handler")
    handler:remove_device(params.deviceAddress)
end
