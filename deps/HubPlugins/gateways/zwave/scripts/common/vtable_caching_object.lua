local object = { }
local vtables = { }
local object_metatable = { __index = object }
local vtable_metatable = {
    __index = function(t, k)
        local result = loadfile(string.format("%s/%s", rawget(t, "__path"), k)) or ( rawget(t, "__base") and rawget(t, "__base")[k] )
        t[k] = result
        return result
    end
}

function object._load(class_name, class_location, ...)
    assert(type(class_name) == 'string', "'class_name' must be a string")
    assert(type(class_location) == 'string', "'class_location' must be a string")

    local instance_initializer = hubplugins.weak_loadfile(string.format("%s/%s/__init__", class_location, class_name))

    if not instance_initializer then
        return instance_initializer
    end

    local instance = setmetatable({}, object_metatable)

    return instance, instance_initializer(instance, ...)
end


function object:get_method(method_name)
    return self._vtable[method_name]
end


function object:call(method_name, ...)
    local method = self._vtable[method_name]
    return method(self, ...)
end


function object:_super_call(class_name, method_name, ...)
    local method = self._vtable._class_names[class_name][method_name]
    return method(self, ...)
end

function object:_init_base(class_name, class_location, ...)
    local base_initializer = hubplugins.weak_loadfile(string.format("%s/%s/__init__", class_location, class_name))

    if not base_initializer then
        error(string.format("class '%s' does not exist at location '%s'", class_name, class_location))
    end

    return base_initializer(self, ...)
end

function object:_register(class_name, class_location)
    local path = string.format("%s/%s", class_location, class_name)
    local vtable = vtables[path]
    if not vtable then
        vtable = { __path = path }
        vtable._class_names = { [class_name] = vtable }

        if self._vtable then
            vtable.__base = self._vtable
            setmetatable(vtable._class_names, {
                    __index = self._vtable._class_names
                })
        end

        vtables[path] = setmetatable(vtable, vtable_metatable)
    end

    self._vtable = vtable
end


return setmetatable(object, { __call = function(_, ...) return object._load(...) end })
