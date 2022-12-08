local parent = ...

local child = setmetatable({}, {
    __index = function(self, key)
        local result = rawget(self, key)
        if result ~= nil then
            return result
        end
        return parent[key]
    end
})

parent.child_devices = parent.child_devices or {}
parent.child_devices[#parent.child_devices + 1] = child

return child
