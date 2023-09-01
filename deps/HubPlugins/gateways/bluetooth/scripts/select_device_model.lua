local params = ...

local storage = require "storage"
if not storage then
    print("Fail to load storage functionality")
    return
end

storage.set_string("selectedDeviceModel", params.deviceModel)
