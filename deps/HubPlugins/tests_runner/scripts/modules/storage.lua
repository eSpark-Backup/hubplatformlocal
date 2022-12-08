local storage = {
    map = {}
}

local tests = require "tests"

function storage.set_string( key, value )
	storage.map[ tostring( key ) ] = tostring(value)
end

function storage.get_string( key )
	return storage.map[ tostring( key ) ]
end

function storage.set_table( key, value )
    assert(value)

    value = tests.deep_copy(value)

    storage.map[ tostring( key ) ] = value
end

function storage.get_table( key )
	return tests.deep_copy(storage.map[ tostring( key ) ])
end

function storage.delete( key )
	storage.map[ tostring( key ) ] = nil
end

function storage.delete_all()
	storage.map = {}
end

function storage.exists( key )
    return storage.map[ tostring( key ) ] ~= nil
end

function storage.get_number( key )
    return tonumber(storage.map[ tostring( key ) ])
end

function storage.set_number( key, value )
    storage.map[ tostring( key ) ] = value
end

function storage.get_bool( key )
    return storage.map[ tostring( key ) ]
end

function storage.set_bool( key, value )
    storage.map[ tostring( key ) ] = value
end

function storage.empty( )
    return next(storage.map) == nil
end

return storage
