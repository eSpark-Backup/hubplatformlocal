local storage = require "storage"
if not storage then
    print("Fail to load storage functionality")
    return
end

local mapper = {}

function mapper.add_char_info(self, deviceAddress, charUUID, itemsInfo)
    local descriptor = self:create_descriptor(deviceAddress, charUUID)

    local charInfo = {}
    for itemId, itemConfig in pairs(itemsInfo) do
        charInfo[itemId] = {
            deviceAddress = deviceAddress,
            setter = itemConfig.setter,
            getter = itemConfig.getter,
        }

        storage.set_string(itemId, descriptor)
    end

    storage.set_table(descriptor, charInfo)
end

function mapper.remove_char_info(self, deviceAddress, charUUID)
    local descriptor = self:create_descriptor(deviceAddress, charUUID)

    local charInfo = storage.get_table(descriptor)
    for itemId, _ in pairs(charInfo) do
        storage.delete(itemId)
    end

    storage.delete(descriptor)
end

function mapper.get_item_info_by_id(self, itemId)
    local descriptor = storage.get_string(itemId)
    local charInfo = storage.get_table(descriptor)
    return charInfo[itemId]
end

function mapper.get_device_info_by_address(self, deviceAddress)
    return storage.get_table(deviceAddress)
end

function mapper.add_device_info(self, deviceId, deviceAddress, charsInfo)
    storage.set_string(deviceId, deviceAddress)

    local charUUIDs = {}
    for charUUID, itemsInfo in pairs(charsInfo) do
        table.insert(charUUIDs, charUUID)

        self:add_char_info(deviceAddress, charUUID, itemsInfo)
    end

    storage.set_table(deviceAddress, { deviceId = deviceId, charUUIDs = charUUIDs })
end

function mapper.remove_device_info_by_address(self, deviceAddress)
    local deviceInfo = storage.get_table(deviceAddress)

    storage.delete(deviceInfo.deviceId)

    for _, charUUID in ipairs(deviceInfo.charUUIDs) do
        self:remove_char_info(deviceAddress, charUUID)
    end

    storage.delete(deviceAddress)
end

function mapper.create_descriptor(self, deviceAddress, charUUID)
    return deviceAddress .. "_" .. charUUID
end

return mapper
