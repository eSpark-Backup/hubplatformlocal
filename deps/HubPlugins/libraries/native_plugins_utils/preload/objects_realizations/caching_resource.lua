local function load_field(path, t, k)
    local file = loadfile(path .. k)
    local result = file and file()

    t[k] = result
    return result
end

return setmetatable({}, {
    __call = function(_, path, initializer)

        return setmetatable(initializer or {}, {
            __index = function(t, k)
                return load_field(path, t, k)
            end
        })
    end
})
