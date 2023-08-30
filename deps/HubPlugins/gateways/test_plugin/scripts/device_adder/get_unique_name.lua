local name = ...

local storage = require "storage"

local names_count = storage.get_number( name )
if not names_count then
    storage.set_number( name, 1 )
    return name
else
    names_count = names_count + 1
    storage.set_number( name, names_count )
    return name .. "_" .. tostring(math.floor(names_count))
end
