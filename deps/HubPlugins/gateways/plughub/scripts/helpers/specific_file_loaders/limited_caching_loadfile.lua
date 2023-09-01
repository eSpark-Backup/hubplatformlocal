--
-- Provides files loading via LRU cache principles.
--
-- Actually it doesn't support "require".
-- If we need this type of caching, extend it for "require" and modify or create new object model.
-- 


local cache = { }
local weak_cache = setmetatable( { }, { __mode = "v" } )
local methods_sizes = setmetatable( { }, { __mode = "k" } )

local unexisting_files = { }

setmetatable( methods_sizes, methods_sizes )

local max_size = 2000

local cache = loadfile("HUB:plughub/scripts/helpers/create_linked_table")()

local current_size = 0

local function loadfile_impl(filename)
    collectgarbage("stop") 
    local prev_count = collectgarbage("count")
    local method = loadfile(filename)
    local file_size = collectgarbage("count") - prev_count
    collectgarbage("restart") 

    if method == nil then
        unexisting_files[ filename ] = true
        return nil
    end

    methods_sizes[method] = math.max(file_size, 0.1)
    weak_cache[filename] = method
    return method
end

return function(filename)
    if unexisting_files[filename] then
        return nil
    end

    local method = cache[filename]
    if method ~= nil then
        cache[filename] = method
        return method
    end

    method = weak_cache[ filename ] or loadfile_impl( filename )

    if method == nil then
        return nil
    end

    cache[filename] = method
    current_size = current_size + methods_sizes[method]

    local it
    while current_size > max_size do
        it = it or pairs(cache)
        local fn, m = it(cache)
        current_size = current_size - methods_sizes[m]
        cache[fn] = nil
    end

    return method
end