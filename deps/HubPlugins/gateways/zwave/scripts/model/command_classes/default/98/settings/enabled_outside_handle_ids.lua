local self, capability = ...

local enum = {}
local value_default = {}
for _, id in ipairs(capability) do
    id = tostring(math.floor(id))
    enum[id] = {
        text = "Handle " .. id
    }
    table.insert(value_default, id)
end

return {
    name = "Supported outside handle ids",
    description = "Outside handle ids that can be managed from door lock configuration",
    value_type = "array.token",
    value = { },
    value_default = value_default,
    enum = enum
}
