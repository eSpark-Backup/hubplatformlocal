local bluetooth = require "bluetooth"
if not bluetooth then
    print("Fail to load bluetooth functionality")
    return
end

bluetooth.subscribe("HUB:bluetooth/scripts/events_handling")
