local cache = setmetatable( { }, {
    __index = function(t, k)
        local method = loadfile(k)
        t[k] = method and method() or false
        return t[k]
    end
} )

return function(filename)
    return cache[filename] or nil
end
