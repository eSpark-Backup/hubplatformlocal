local cache = setmetatable( { }, {
    __index = function(t, k)
        local method = loadfile(k)
        t[k] = method
        return method
    end,
    __mode = "v"
} )

return function(filename)
    return cache[filename]
end
