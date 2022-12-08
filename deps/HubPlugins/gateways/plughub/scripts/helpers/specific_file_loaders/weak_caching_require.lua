local weak_cache = setmetatable( { }, { __mode = "v" } )

local function require_impl(filename)
    local method = plughubplugins.weak_loadfile(filename)
    local result = method and method()
    weak_cache[filename] = result
    return result
end

return function(filename)
    return weak_cache[filename] or require_impl(filename)
end
