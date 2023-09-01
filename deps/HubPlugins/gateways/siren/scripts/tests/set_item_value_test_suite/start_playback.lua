local core = require("core")
local storage = require("storage")
local speaker = require("speaker")
local tests = require("tests")
local log = require("logging")

print('Log level is set to: ', log.get_level())

assert(loadfile("HUB:siren/scripts/startup")())
local logger = assert(loadfile("HUB:siren/scripts/utils/log")())
local switch_enum = require('HUB:siren/scripts/items/switch_enum')

local function find_item(items, item_name)
    for _, item in pairs(items) do
        log.trace('Item name: ' .. item.name)
        if item.name == item_name then
            return item.id
        end
    end
    return nil
end

local devices = core.get_devices()
assert(#devices == 1, "Should be one device")
local device_id = devices[1].id
local items = core.get_items_by_device_id(device_id)
log.trace(#items)

local item_name = 'switch'

local item_id = find_item(items, item_name)
log.trace(item_id)
assert(item_id, "Should found switch item")

loadfile("HUB:siren/scripts/set_item_value")({
    item_id = item_id,
    value = switch_enum.play
})

local playback = core.get_item(item_id)
log.trace("switch.value: " .. tostring(playback.value))

local command_id = storage.get_string("speaker_command_id")
log.trace("storage.speaker_command_id:", command_id)

local info = speaker.get_playing_info(command_id)
tests.assert_ne(info, nil, "Playing info should not be null")
log.trace(info.status)
logger.trace_dump(info, "playingInfo")
tests.assert_eq(info.status, 'playing', "Playing info.status should be 'playing'")
tests.assert_eq(command_id, "1")

speaker.stop(command_id)
local stopped_info = speaker.get_playing_info(command_id)
tests.assert_ne(stopped_info, nil, "Playing info should not be null")
logger.trace_dump(stopped_info, "stoppedInfo")
tests.assert_eq(stopped_info.status, 'stopped', "Playing info.status should be 'playing'")
