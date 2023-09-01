local is_array = loadfile("HUB:zwave/scripts/helpers/is_array")

local tests = {
    skipped_param = {} -- use skipped_param to avoid some fields comparison
}

function tests.deep_copy(orig, copies)
    copies = copies or {}
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        if copies[orig] then
            copy = copies[orig]
        else
            copy = {}
            copies[orig] = copy
            for orig_key, orig_value in next, orig, nil do
                copy[tests.deep_copy(orig_key, copies)] = tests.deep_copy(orig_value, copies)
            end
            setmetatable(copy, tests.deep_copy(getmetatable(orig), copies))
        end
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

function tests.assert_eq( actual, expected, description )

    tests.deep_compare( actual, expected,
        function( actual_field, expected_field, keys_stack )
            local keys_string
            for _, key in ipairs( keys_stack ) do
                keys_string = keys_string and ( keys_string .. "." .. tostring( key ) ) or ( "Field: " .. tostring( key ) )
            end

            error( "assert_eq test '" .. ( description or "" ) .. "' failed. " ..
                    "AR: '" .. tostring( actual_field ) .. "' " ..
                    "ER: '" .. tostring( expected_field ) .. "' " ..
                    ( keys_string or "" ) )
        end )

    return actual
end

function tests.assert_ne( actual, unexpected, description )
    local ne_handler = tests.create_mock()
    tests.deep_compare( actual, unexpected, ne_handler )

    assert( next( ne_handler.call_params ), "assert_ne test '" .. ( description or "" ) .. "' failed. " ..
                                            "AR: '" .. tostring( actual ) .. "'" )
    return actual
end

---
--- @Usage
---
--- local function single_param_test( product_id )
---    tests.assert_ne( product_id, nil, "check product id" )
--- end
---
--- tests.run_with_params( single_param_test,
--- {
---     {
---         value = 2,
---         description = "test with product id == 2"
---     },
---     {
---         value = 3,
---         description = "test with product id == 3"
---     }
--- } )
---
--- local function multi_param_test( product_id, product_type )
---    zwave.add_node({
---        id = 1,
---        product_id = product_id,
---        product_type = product_type
---    })
---
---    events_handler( {
---       event = "node_added",
---       nodeId = 1
---    } )
---
---    tests.assert_eq( #core.get_devices(), 0, "check no added devices independ of product id and product_type" )
--- end
---
---
--- tests.run_with_params( multi_param_test, -- runs with each product_id and each product_type ( 2x2=4 executions )
--- {
---     {
---         value = 2,
---         description = "test with product id == 2"
---     },
---     {
---         value = 3,
---         description = "test with product id == 3"
---     }
--- },
--- {
---     {
---         value = 10,
---         description = "test with product type == 10"
---     },
---     {
---         value = 11,
---         description = "test with product id == 11"
---     }
--- } )
function tests.run_with_params( f, ... )
    local param_groups = { ... }

    local function helper( description, ... )
        local size = select( '#', ... )
        if size == #param_groups then
            print("Run param test:" .. description)
            f( ... )
        else
            for _, prm in ipairs( param_groups[ #param_groups - size ] ) do
                helper( " " .. ( prm.description or tostring( prm.value ) ) .. ";" .. description,
                        prm.value,
                        ... )
            end
        end
    end

    helper("")
end

function tests.track_calls(original)
    local result = {
        call_params = {}
    }

    setmetatable( result, {
        __call = function( _, ... )
            table.insert( result.call_params, { ... } )
            return original(...)
        end
    })

    return result
end

function tests.create_mock(return_value)
    local result = {
        call_params = {}
    }

    setmetatable( result, {
        __call = function( _, ... )
            table.insert( result.call_params, { ... } )
            return return_value
        end
    })

    return result
end

function tests.deep_compare( a, b, ne_handler )
    local function deep_compare_impl( keys_stack, a, b )
        if a == b or a == tests.skipped_param or b == tests.skipped_param then
            return
        elseif type( a ) == "table" and type( b ) == "table" then
            local function recursive_call_for_member( key, a, b )
                table.insert( keys_stack, key )
                deep_compare_impl( keys_stack, a, b )
                table.remove( keys_stack, #keys_stack )
            end

            for key, value in pairs( a ) do
                recursive_call_for_member( key, value, b[ key ] )
            end

            for key, value in pairs( b ) do
                if a[ key ] == nil then
                    recursive_call_for_member( key, nil, value )
                end
            end
        else
            ne_handler( a, b, keys_stack )
        end
    end

    deep_compare_impl( {}, a, b )
end

---
--- @Usage
---
--- zwave.request_value = tests.create_mock() -- create mock for call expectations
---
--- -- Call expectation examples:
---
--- tests.assert_called( zwave.request_value, "check request value called once" )
---
--- tests.assert_called( zwave.request_value, "check request value called 5 times", nil, 5 )
---
--- tests.assert_called( zwave.request_value,
---                      "check request value called 2 times with custom checker",
---                      function(call_index, node_id, class_id, channel_id)
---                          tests.assert_ne( cc_ids_set[class_id], nil, "check cc id is present in set" )
---                      end,
---                      2 )
---
--- tests.assert_called( zwave.request_value,
---                      "check request value called 2 times with params",
---                      { { "2", 48, 0 }, { "2", 156, 0 } } )
---
--- tests.assert_called( zwave.request_value,
---                      "check request value called once with checking of first and third params",
---                      { { "2", tests.skipped_param, 0 } } )
function tests.assert_called( mock, description, checker, call_count )
    assert( mock, "nil mock is passed to tests.assert_called" )

    description = description or "assert_mock_called"

    local call_params = mock.call_params
    mock.call_params = { }

    local check_function

    if type( checker ) == "table" then
        assert( call_count == nil, "call_count is ignored if table passed as checker" )
        call_count = #checker

        check_function = function( call_index, ... )
            local expected_params = checker[ call_index ]
            tests.assert_eq( type( expected_params ), "table", "expected params must be table" )
            tests.assert_eq( select( '#', ... ), #expected_params, "check function params count" )

            assert(is_array(checker[ call_index ]), "checker " .. call_index .. " must be contiguous array")

            for param_index, expected in ipairs( checker[ call_index ] ) do
                tests.assert_eq( select( param_index, ... ),
                                 expected,
                                 description .. " : check call param " .. param_index .. " of call " .. call_index )
            end
        end
    else
        check_function = type( checker ) == "function" and checker or function() end
        call_count = call_count or 1
    end

    tests.assert_eq( #call_params, call_count, description .. " : check calls count" )

    for call_index, params in ipairs( call_params ) do
        check_function( call_index, table.unpack( params ) )
    end
end

function tests.assert_called_unordered( mock, description, checker )
    assert( mock, "nil mock is passed to tests.assert_called" )

    description = description or "assert_called_unordered"

    local call_params = mock.call_params
    mock.call_params = { }

    tests.assert_eq( #call_params, #checker, description .. " : check calls count" )

    for expected_param_index, expected_params in ipairs( checker ) do
        local found_index
        for index, actual_params in ipairs( call_params ) do
            local result = pcall( tests.deep_compare, actual_params, expected_params, function() error("") end )
            if result then
                found_index = index
                break
            end
        end
       
        if found_index == nil then
            error(description .. " : check unordered call param " .. expected_param_index)
        else
            table.remove(call_params, found_index)
        end
    end

    tests.assert_eq( #call_params, 0, description .. " : mock must be cleared" )
end

function tests.assert_called_postponed( add_time_func, delay, mock, ... )
    if delay > 0 then
        local delta = delay > 1 and 1 or delay / 2

        add_time_func(delay - delta)
        tests.assert_called(mock, "check not called before postponed call", { })

        add_time_func(delta)
    else
        add_time_func(0)
    end

    tests.assert_called(mock, ...)
end

function tests.assert_unique( array, description )
    if not array then
        return nil
    end

    local result = {}
    for _, val in ipairs(array) do
        tests.assert_eq(result[ val ], nil,
                        "value " .. tostring(val) .. " in " ..
                        (description and description or "array") ..
                        " is not unique")
        result[ val ] = true
    end
    return result
end



---
--- @Usage
---
--- tests.assert_failed( failed_function )
---
--- tests.assert_failed( failed_function, arg1 )
---
--- tests.assert_failed( "Check fail of our function with two args", failed_function, arg1, arg2 )
---
function tests.assert_failed( description_or_func, ... )
    if type(description_or_func) == "string" then
        assert( not pcall( ... ), description_or_func .. ": Function must fail" )
    elseif type(description_or_func) == "function" then
        assert( not pcall( description_or_func, ... ), "Function must fail" )
    else
        error(("Unknown arg: %s"):format(description_or_func))
    end
end

---
--- @Usage
---
--- tests.override_function( _G, "loadfile", function(orig_loadfile, path) )
---     if path == "mypath" then
---         print("do some custom action")
---     end
---     return orig_loadfile(path)
--- end )
---
--- tests.reset_overriden_function(_G, "loadfile")
---
function tests.override_function( context, function_name, new_function )
    context = context or _G
    assert( context[function_name] )
    assert( type(new_function) == "function" )

    context[function_name] = setmetatable(
    {
        _orig_function = context[function_name]
    },
    {
        __call = function(t, ...)
            return new_function(t._orig_function, ...)
        end
    })

    return context[function_name]._orig_function
end

function tests.reset_overriden_function( context, function_name )
    context = context or _G
    if type(context[function_name]) == "table" and context[function_name]._orig_function then
        context[function_name] = context[function_name]._orig_function
    end
end

return tests
