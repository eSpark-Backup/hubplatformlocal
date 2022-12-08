--
-- @Brief
--
-- Linked table is key-value map with defined order of elements. Each new or changed element becomes last. 
-- Use standard operator [] to set/get value.
-- Use pairs function for iteration.
--
-- TODO: 
-- Some helpful functions like next, prev, insert_before are not implemented actually.
-- If they needed, implement object linked_table with static functions that implement required interface.
-- After refactoring please run performance test (see linked_table_test).
-- 


local first
local last
local nodes = {}

-- Usege of node as array of 3 elements is more optimal for memory and performance
local value = 1
local prev_key = 2
local next_key = 3

return setmetatable(
{},
{
    __next = function(t, k)
        if k == nil then
            k = first
        else
            k = nodes[k][next_key]
        end

        if k == nil then
            return nil, nil
        else
            return k, nodes[k][value]
        end
    end,
    __pairs = function(t)
        return getmetatable(t).__next, t, nil
    end,
    __index = function(_, k)
        local node = nodes[k] 
        return node and node[value]
    end,
    __newindex = function(t, k, v)
        local node = nodes[k]
        if node then
            if node[prev_key] then
                nodes[node[prev_key]][next_key] = node[next_key]
            else
                first = node[next_key]
            end

            if node[next_key] then
                nodes[node[next_key]][prev_key] = node[prev_key]
            else
                last = node[prev_key]
            end
        end

        if v ~= nil then
            if last ~= nil then
                nodes[last][next_key] = k
            else
                first = k
            end

            if node then
                node[value] = v
                node[prev_key] = last
                node[next_key] = nil
            else
                nodes[k] = { v, last }
            end

            last = k
        else
            nodes[k] = nil
        end
    end
})
