local tests = require("tests")

local impl = {
    commands = {},
    counter = 1,
    events = {},
    current_event = 0
}
function impl.add_command(playing_info)
    local command_id = impl.counter
    impl.commands[tostring(impl.counter)] = playing_info

    return tostring(command_id)
end
function impl.playing_info(command_id)
    return impl.commands[command_id]
end

function impl.emit(event)
    tests.assert_eq(type(event), "table", "Event should be a table")
    tests.assert_eq(type(event.id), "string", "Event.is should be a string")

    if impl.events[tostring(event.id)] == nil then
        impl.events[tostring(event.id)] = {}
    end
    local command_events = impl.events[tostring(event.id)]
    command_events[#command_events + 1] = event
end

local speaker = {
    impl = impl
}

function speaker.play( playing_info )
    assert(type(playing_info) == 'table', 'Argument should be a table' )
    assert(playing_info.path ~= nil and type(playing_info.path) == 'string',
        'Argument should have required field: path: string' )
    if playing_info.volume ~= nil then
        assert(type(playing_info.volume) == 'number', 'playing_info.volume should be a number')
    end
    if playing_info.time ~= nil then
        assert(type(playing_info.time) == 'number', 'playing_info.time should be a number')
    end
    if playing_info.handler ~= nil then
        assert(type(playing_info.handler) == 'string', 'playing_info.handler should be a string')
    end
    if playing_info.user_data ~= nil then
        -- can be any type
    end

    local command_id = speaker.impl.add_command(playing_info)
    local event = {
        id = command_id,
        status = 'playing',
        user_data = playing_info.user_data or nil
    }
    speaker.impl.emit(event)
    return command_id
end

function speaker.stop(command_id)
    local status, info = pcall(impl.playing_info, command_id)
    if status then
        local event = {
            id = command_id,
            status = 'stopped',
            user_data = info.user_data or nil
        }
        speaker.impl.emit(event)
    end
end

function speaker.get_playing_info(command_id)
    assert(command_id, 'Field command_id is empty')
    local status, info = pcall(impl.playing_info, command_id)
    if status then
        -- iterate events
        local last_event = {}
        local container = impl.events[command_id] or {}
        for _, event in pairs(container) do
            if event.id == command_id then
                last_event = tests.deep_copy(event)
            end
        end
        local last_status = "pending"
        if last_event ~= nil then
            last_status = last_event.status
        end

        -- make playingInfo
        local playing_info = {
            id = command_id,
            path = info.path or "",
            volume = info.volume or 100,
            user_data = info.user_data,
            status = last_status,
        }

        return playing_info
    end
    return nil
end

-- event speaker_playing_info

return speaker