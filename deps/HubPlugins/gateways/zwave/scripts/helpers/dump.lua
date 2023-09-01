local object, dump_metatable = ...

if dump_metatable == nil then
    dump_metatable = true
end

local is_array = loadfile('HUB:zwave/scripts/helpers/is_array')

-- Note: will throw an error if it can't compare values with the '<' operator (e.g. tables as keys).
-- For such cases you need to pass a custom copmarator as the 2nd argument (or simply use pairs instead).
local function sorted_pairs(obj, comparator)
    local keys = {}
    for key in pairs(obj) do
        keys[#keys + 1] = key
    end
    table.sort(keys, comparator)

    local idx = 0
    return function()
        idx = idx + 1
        local key = keys[idx]
        if key ~= nil then
            return key, obj[key]
        end
    end
end

local indent_level = 0
local stack = {}

local function newline()
    return '\n' .. string.rep('    ', indent_level)
end

local dump_key, dump_value, dump_table, dump_table_reference

dump_key = function(obj)
    if type(obj) == 'string' then
        return obj
    elseif type(obj) == 'table' then
        return ('[%s]'):format(dump_table(obj))
    else
        return ('[%s]'):format(tostring(obj))
    end
end

dump_value = function(obj)
    if type(obj) == 'table' then
        return dump_table(obj)
    elseif type(obj) == 'string' then
        return ('"%s"'):format(obj)
    else
        return tostring(obj)
    end
end

local function get_tokens(obj)
    local tokens = {}

    if is_array(obj) then
        for _, value in ipairs(obj) do
            tokens[#tokens + 1] = dump_value(value)
        end
    else
        for key, value in sorted_pairs(obj) do
            tokens[#tokens + 1] = ('%s = %s'):format(dump_key(key), dump_value(value))
        end
    end

    if getmetatable(obj) and dump_metatable then
        tokens[#tokens + 1] = '__metatable = ' .. dump_value(getmetatable(obj))
    end

    return tokens
end

dump_table = function(obj)
    if stack[obj] ~= nil then
        return dump_table_reference(obj)
    elseif next(obj) == nil and not (dump_metatable and getmetatable(obj)) then
        return '{}'
    else
        stack[obj] = true -- push
        indent_level = indent_level + 1

        local stream = '{' .. newline() .. table.concat(get_tokens(obj), ',' .. newline())

        indent_level = indent_level - 1
        stack[obj] = nil -- pop

        return stream .. newline() .. '}'
    end
end

dump_table_reference = function(obj)
    return ('{ --[[%s]] }'):format(tostring(obj))
end

return dump_value(object)
