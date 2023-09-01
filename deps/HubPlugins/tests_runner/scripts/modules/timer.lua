local timer = { }

local tests = require "tests"

local timer_infos = {}
local now = 0
local timer_id = 0

local function find_timer_info(id)
    tests.assert_eq(type(id), "string", "timer id type must be string")
    for index, info in ipairs(timer_infos) do
        if info.id == id then
            return index, info
        end
    end
end

local function get_next_id()
    timer_id = timer_id + 1
    return "test_timer_" .. timer_id
end

local function add_timer(timeout, id, script_name, script_args, periodic)
    tests.assert_eq(type(timeout), "number", "type of timeout (" .. tostring(timeout) .. ") must be number")
    assert(timeout >= 0)

    if not find_timer_info(id) then
        print("add timer " .. id)
        table.insert(timer_infos, {
                id = id,
                timeout = now + timeout,
                script_name = script_name,
                script_args = tests.deep_copy(script_args),
                periodic = periodic
            })

        return id
    else
        print("cannot add timer, " .. id .. " is already present")
    end
end

function timer.set_timeout_with_id( timeout, id, script_name, script_args )
    add_timer( timeout, id, script_name, script_args )
end

function timer.set_timeout( timeout, script_name, script_args )
    return add_timer( timeout, get_next_id(), script_name, script_args )
end

function timer.set_interval_with_id( timeout, id, script_name, script_args )
    add_timer( timeout, id, script_name, script_args, true )
end

function timer.set_interval( timeout, script_name, script_args )
    return add_timer( timeout, get_next_id(), script_name, script_args, true )
end

function timer.exists( id )
    return find_timer_info( id ) ~= nil
end

function timer.reset( timeout, id )
    tests.assert_eq(type(timeout), "number", "type of timeout must be number")
    assert(timeout >= 0)
    
    local index, info = find_timer_info( id )
    if index then
        print("reset timer " .. id)

        table.remove( timer_infos, index )
        info.timeout = now + timeout
        table.insert( timer_infos, info )
    else
        print( "timer.reset: id " .. id .. " not found" )
    end
end

function timer.cancel( id )
    print("cancel timer " .. id)
    local index = find_timer_info( id )
    if index then
        
        table.remove( timer_infos, index )
    end
end

function timer.get_arguments( id )
    local index, info = find_timer_info( id )
    return info and info.script_args
end

function timer.reset_arguments( id, script_args )
    local index, info = find_timer_info( id )
    if info then
        info.script_args = script_args
    else
        print("timer.reset_arguments: id " .. id .. " not found")
    end
end

function timer.add_time( time )
    local next_timepoint = now + time

    while true do
        local next_info_index, next_info
        for index, info in ipairs(timer_infos) do
            if not next_info_index or info.timeout < next_info.timeout then
                next_info_index = index
                next_info = info
            end
        end

        if not next_info_index or next_timepoint < next_info.timeout then
            break
        end

        table.remove(timer_infos, next_info_index)

        if next_info.id == "last_test_timer_id" then
            break
        end

        local script_args = tests.deep_copy(next_info.script_args)
        if script_args then
            script_args.timer_id = next_info.id
        end

        now = next_info.timeout

        print("run timer " .. next_info.id .. " handler " .. next_info.script_name .. ", now " .. now)
        hubplugins.weak_loadfile(next_info.script_name)(script_args)

        if next_info.timeout == next_timepoint then
            -- avoid handling of new timers with zero timeout
            table.insert(timer_infos, { id = "last_test_timer_id", timeout = next_timepoint })
        end
    end

    now = next_timepoint
end

function timer.get_count()
    return #timer_infos
end

function timer.get_time_to_execution( id )
    local index, info = find_timer_info( id )
    return info and info.timeout - now
end

return timer