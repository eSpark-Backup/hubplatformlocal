local tests = require("tests")
local extensions_manager = require "extensions_manager"

local fixture = {
    extensions = {},
    extensions_data = {}
}

function fixture.init()
    extensions_manager.get_plugin_extensions = fixture.get_plugin_extensions
    extensions_manager.get_plugin_extension_data = fixture.get_plugin_extension_data
end

function fixture.add_extension_data(extension_name, data)
    table.insert(fixture.extensions, extension_name)
    fixture.extensions_data[extension_name] = data
end

function fixture.get_plugin_extensions()
    return tests.deep_copy(fixture.extensions)
end

function fixture.get_plugin_extension_data(plugin_id, path, filter)
    local path_fields = fixture.split("/", path)
    if fixture.extensions_data[path_fields[1]] then
        local current_field = fixture.extensions_data[path_fields[1]]
        for i = 3, #path_fields do
            current_field = current_field[path_fields[i]]
            if not current_field then return fixture.return_default(filter) end
        end
        
        if filter == "keys" then
            local result = {}
            for key,_ in pairs(current_field) do
                table.insert(result, key)
            end
            return result
        elseif filter == "count" then
            return #current_field
        elseif tonumber(filter) ~= nil then
            return current_field[tonumber(filter) + 1]
        else
            return tests.deep_copy(current_field)
        end
    end
    
    return fixture.return_default(filter)
end

function fixture.return_default(filter)
    if filter == "count" then return 0 
    else return {} end
end

function fixture.split(sep, source)
   local fields = {}
   local pattern = string.format("([^%s]+)", sep)
   source:gsub(pattern, function(c) fields[#fields+1] = c end)
   return fields
end

return fixture
