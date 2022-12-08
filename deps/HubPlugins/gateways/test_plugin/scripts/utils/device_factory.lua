local prototype = ...

local function deep_copy(obj)
    local new_obj = {}
    for k, v in pairs(obj) do
        new_obj[k] = v
    end
    return new_obj
end

local device_factory = {}

function device_factory:create_device(items)
    local device = deep_copy(prototype)

    device.name = table.concat({ prototype.name, self.idx }, " ")
    device.items = items

    self.idx = self.idx ~= nil and self.idx + 1 or 1

    return device
end

return device_factory
