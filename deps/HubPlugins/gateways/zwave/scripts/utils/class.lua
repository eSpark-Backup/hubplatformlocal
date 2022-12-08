local class_module = {}


local function create_class(supper)
    assert(supper == nil or type(supper) == 'table', "'super' may be omitted otherwise it must be a table")

    local class = {}
    local meta_class = {}

    class.__index = class

    meta_class.__index = supper

    function meta_class:__call(...)
        local instance = setmetatable({}, class)

        if type(class.init) == 'function' then
            return instance, instance:init(...)
        end

        return instance
    end

    return setmetatable(class, meta_class)
end


setmetatable(class_module, { __call = function(_, ...) return create_class(...) end })

return class_module
