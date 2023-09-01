local params = ...

local bluetooth = require "bluetooth"
if not bluetooth then
    print("Fail to load bluetooth functionality")
    return
end

bluetooth.maintain_connection(params.deviceAddress)
