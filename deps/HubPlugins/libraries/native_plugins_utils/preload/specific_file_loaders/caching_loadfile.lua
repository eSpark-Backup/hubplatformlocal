local cache = setmetatable( { }, {
    __index = function(t, k)
        t[k] = loadfile(k) or false
        return t[k]
    end
} )

return function(filename)
    return cache[filename] or nil
end
