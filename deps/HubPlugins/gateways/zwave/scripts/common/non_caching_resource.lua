return setmetatable({}, { 
    __call = function(_, path, initializer) 
        return setmetatable(initializer or {}, {
            __index = function(t, k)
                local file = loadfile(path .. k)
                return file and file()
            end
        })
    end 
})
