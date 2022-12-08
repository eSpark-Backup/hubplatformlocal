local self, callback = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

self:call(
    "utils/for_each_root_device",
    function(device)
        return device.category == "hvac"
    end,
    function(device)
        local info = self:call("utils/get_device_energy_saving_info", device.id)
        if info then
            callback(info)
        end
    end
)
