local core = require "core"
if not core then
    print("Fail to load core functionality")
    return
end

local handler = {
    gateway_id = "unknown",
    name = "unknown",
    type = "device",
    device_type_id = "",
    category = "unknown"
}

function handler.add_device(self, deviceAddress, deviceModel)
    local characteristics = require("HUB:bluetooth/scripts/devices/" .. deviceModel .. "/characteristics")
    if not characteristics then
        print(string.format("No available characteristics for device '%s' model '%s'", deviceAddress, deviceModel))
        return
    end

    local deviceInfo = self
    local deviceId = core.add_device(deviceInfo)

    local charsInfo = {}
    for _, charUUID in ipairs(characteristics) do
        charsInfo[charUUID] = self:process_characteristic(deviceId, charUUID)
    end

    local mapper = require("HUB:bluetooth/scripts/utils/mapper")
    mapper:add_device_info(deviceId, deviceAddress, charsInfo)
end

function handler.remove_device(self, deviceAddress)
    local mapper = require("HUB:bluetooth/scripts/utils/mapper")
    local deviceInfo = mapper:get_device_info_by_address(deviceAddress)
    if deviceInfo then
        core.remove_device(deviceInfo.deviceId)
        mapper:remove_device_info_by_address(deviceAddress)
    else
        print(string.format("Can't remove device '%s', maybe it wasn't added", deviceAddress))
    end
end

function handler.process_characteristic(self, deviceId, charUUID)
    local itemsInfo = {}

    local itemsData = require("HUB:bluetooth/scripts/devices/16EFB0A/" .. charUUID .. "/items")
    for _, data in ipairs(itemsData) do
        local item = data.item
        local config = data.config

        item.device_id = deviceId
        item.has_getter = (config["getter"] ~= nil)
        item.has_setter = (config["setter"] ~= nil)

        local id = core.add_item(item)
        itemsInfo[id] = config
    end

    return itemsInfo
end

return handler
