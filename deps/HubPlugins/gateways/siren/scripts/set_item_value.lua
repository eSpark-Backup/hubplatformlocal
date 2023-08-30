-- INFO : bridge between speaker lua api and siren plugin's device/items
-- Call: set_item_value { item_id, value }
local params = ...
assert(params, "Parameters are empty")

local speaker = assert(require("speaker"), 'Failed to load "speaker" module')
local logger = require("HUB:siren/scripts/utils/log")
local storage = assert(require "storage", "Failed to load storage")
local core = assert(require "core", "Failed to load core")
local storage_keys_getter = loadfile("HUB:siren/scripts/utils/storage_keys_getter")()
local item_names = loadfile("HUB:siren/scripts/items/names")()

local switch_enum = require('HUB:siren/scripts/items/switch_enum')

local function trace(...)
    logger.trace("set_item_value.lua: ", ...)
end

local function select_sound_file(index)
    -- TODO: choose file using sound_list/sound_select items
    index = index or "1"
    local map_sound_files = {
        ["1"] = "HUB:siren/data/alarm.wav"
    }
    return map_sound_files[index]
end

local function plugin_event_handler()
    return "HUB:siren/scripts/event_handler"
end

local function select_playback_volume()
    return 100
end

local function select_playback_time()
    return -1
end

local function speaker_play()
    local key_command_id = storage_keys_getter.get_speaker_command_id()
    trace("Start playing a sound")

    local play_command = {
        path = select_sound_file() or "",
        volume = select_playback_volume() or 100,
        time = select_playback_time(),
        handler = plugin_event_handler(),
        user_data = {
            item_id = params.item_id
        }
    }
    local command_id = speaker.play(play_command)

    if command_id ~= nil then
        -- Store speaker_command_id to a storage as a current playing command identifier
        storage.set_string(key_command_id, command_id)
    else
        logger.error('speaker.play returned nil')
    end
end

local function speaker_stop()
    -- TODO: change with state
    local key_command_id = storage_keys_getter.get_speaker_command_id()
    local status, command_id = pcall(storage.get_string, key_command_id)
    if status and command_id ~= nil then
        trace("Stop playing the sound, speaker command id: ", command_id)

        speaker.stop(command_id)
    else
        local error_message = 'Could not find command_id in a storage'
        logger.error(error_message)
    end
end

local function set_siren_playback(playback_state)
    assert(speaker, "Invalid speaker argument")
    assert(playback_state ~= nil, "Invalid params argument: " .. tostring(playback_state))

    if playback_state == switch_enum.play then
        speaker_play()
    elseif playback_state == switch_enum.stop then
        speaker_stop()
    else
        logger.error("Invalid siren_playback item value is passed: ", playback_state)
        error("Invalid siren_playback item value is passed")
    end
end

local function speaker_is_playing()
    local is_playing = false

    -- check if already playing: fetch command_id key from storage, make speaker.get_playing_info() call
    local key_name = storage_keys_getter.get_speaker_command_id()
    local command_id = storage.get_string(key_name)

    trace("Stored command_id ", command_id)
    if command_id ~= nil then
        -- if playing, skip command
        local status, info = pcall(speaker.get_playing_info, command_id)
        if status and info ~= nil and type(info) == "table" and info.status ~= "" then
            trace("Playing info for command_id " .. command_id .. ": ")
            logger.trace_dump(info, "playing_info")

            if info.status ~= nil and info.status == "playing" then
                is_playing = true
                trace("Siren speaker is still playing, skip setting the item value.")
            else
                trace("Siren speaker is not playing, new playback can be started.")
            end
        end
    end

    return is_playing
end

local item_key = storage_keys_getter.get_item_key(item_names.switch)
local stored_value = storage.get_string(item_key)
trace("siren: Stored item_id value: ", item_key, ": ", stored_value)

-- TODO: add code for other 3 items here
trace(stored_value, params.item_id, "isequal: ", stored_value == params.item_id)

if stored_value == params.item_id then
    local playback_item_value = core.get_item(params.item_id)
    logger.trace_dump(playback_item_value, 'siren.switch')

    if playback_item_value ~= nil and playback_item_value.value ~= nil then
        local is_playing = speaker_is_playing()

        local change_siren_state =
            (not is_playing and params.value == switch_enum.play) or
            (is_playing and params.value == switch_enum.stop)
        if change_siren_state then
            set_siren_playback(params.value)
            -- change state
        else
            trace("No need to change siren status")
        end
    end
else
    logger.error("siren: set_item_value, wrong item id " .. params.item_id)
end
