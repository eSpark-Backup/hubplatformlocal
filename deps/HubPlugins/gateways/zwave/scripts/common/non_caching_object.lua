require ("HUB:zwave/scripts/helpers/scripts_preload")

local object = {}

function object._load(class_name, class_location, ...)
    assert(type(class_name) == 'string', "'class_name' must be a string")
    assert(type(class_location) == 'string', "'class_location' must be a string")

    local instance_initializer = hubplugins.weak_loadfile(string.format("%s/%s/__init__", class_location, class_name))

    if not instance_initializer then
        return instance_initializer
    end

    local instance = setmetatable({}, { __index =  object })

    instance._class_names = {}
    instance._paths = {}

    return instance, instance_initializer(instance, ...)
end


function object:get_method(method_name)
    local index_most_derived_class = #self._paths
    return self:_get_method(index_most_derived_class, method_name)
end


function object:call(method_name, ...)
    local index_most_derived_class = #self._paths
    return self:_super_call_impl(index_most_derived_class, method_name, ...)
end


function object:_super_call(class_name, method_name, ...)
    local hierarchy_index = assert(self._class_names[class_name], string.format("class '%s' is not registered", class_name))
    return self:_super_call_impl(hierarchy_index, method_name, ...)
end


function object:_get_method(hierarchy_index, method_name, ...)
    hierarchy_index = math.min( self._top_method_indexes[ method_name ] or hierarchy_index, hierarchy_index )
    for i = hierarchy_index, 1, -1
    do
        local method = hubplugins.weak_loadfile(string.format("%s/%s", self._paths[i], method_name))
        if method then
            if hierarchy_index == #self._paths and i ~= hierarchy_index then
                self._top_method_indexes[ method_name ] = i
            end
            return method
        end
    end
end


function object:_super_call_impl(hierarchy_index, method_name, ...)
    local method = self:_get_method(hierarchy_index, method_name, ...)
    if method then
        return method(self, ...)
    else
        error(string.format("method '%s' is not implemented", method_name), 3)
    end
end


function object:_init_base(class_name, class_location, ...)
    local base_initializer = hubplugins.weak_loadfile(string.format("%s/%s/__init__", class_location, class_name))

    if not base_initializer then
        error(string.format("class '%s' does not exist at location '%s'", class_name, class_location))
    end

    return base_initializer(self, ...)
end


function object:_register(class_name, class_location)
    self._paths[#self._paths + 1] = string.format("%s/%s", class_location, class_name)
    self._class_names[class_name] = #self._paths
    self._top_method_indexes = {}
end


return setmetatable(object, { __call = function(_, ...) return object._load(...) end })
