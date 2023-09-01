local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local sounding_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")
local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")

zwave.request_node_values = tests.create_mock()

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = false,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = {
        {
            id = 0,
            classes = {
                {
                    id = class_ids.sound_switch,
                    tones = {
                        { tone_id = 1 , duration = 5,  tone_name = "01 Ding Dong" },
                        { tone_id = 2 , duration = 9,  tone_name = "02 Ding Dong Tubular" },
                        { tone_id = 3 , duration = 10, tone_name = "03 Traditional Apartment Buzzer" },
                        { tone_id = 4 , duration = 1,  tone_name = "04 Electric Apartment Buzzer" },
                        { tone_id = 5 , duration = 12, tone_name = "05 Westminster Chimes" }
                    }
                }
            }
        }
    }
}

zwave.add_node(node)

events_handler({
    event = "node_added",
    node_id = 9000
})

local function sendSoundSwitchConfigurationUpdated(volume, default_tone_id)
    events_handler({
        event = "sound_switch_configuration_updated",
        node_id = 9000,
        class_id = class_ids.sound_switch,
        channel_id = 0,
        volume = volume,
        default_tone_id = default_tone_id
    })
end

local function sendSoundSwitchTonePlayUpdated(tone_id)
    events_handler({
        event = "sound_switch_tone_play_updated",
        node_id = 9000,
        class_id = class_ids.sound_switch,
        channel_id = 0,
        tone_id = tone_id,
        volume = 0 -- Not used yet
    })
end

local function getItem(itemName)
    local itemId = storage:call("find_item_id", itemName, 9000--[[node_id]], class_ids.sound_switch, 0--[[channel_id]])

    return core.get_item(itemId)
end

-- State is saved between events
local itemValues = {}

local function checkItemValues(newValues)
    for name, value in pairs(newValues) do
        itemValues[name] = value
    end

    for name, value in pairs(itemValues) do
        tests.assert_eq(getItem(name).value, value, ("Check %s value"):format(name))
    end
end

-- Initial
checkItemValues({
    sound_list = {
        ["1"] = { duration = 5,  name = "01 Ding Dong" },
        ["2"] = { duration = 9,  name = "02 Ding Dong Tubular" },
        ["3"] = { duration = 10, name = "03 Traditional Apartment Buzzer" },
        ["4"] = { duration = 1,  name = "04 Electric Apartment Buzzer" },
        ["5"] = { duration = 12, name = "05 Westminster Chimes" }
    },
    sound_playback = sound_playback_enum.unknown,
    sound_select = 1,
    sound_volume = 100,
    sounding_mode = sounding_mode_enum.unknown
})

sendSoundSwitchConfigurationUpdated(0, 1)
checkItemValues({
    sound_volume = 0,
    sounding_mode = sounding_mode_enum.silent
})

sendSoundSwitchConfigurationUpdated(50, 1)
checkItemValues({
    sound_volume = 50,
    sounding_mode = sounding_mode_enum.audible
})

sendSoundSwitchConfigurationUpdated(100, 1)
checkItemValues({
    sound_volume = 100
})

sendSoundSwitchConfigurationUpdated(100, 2)
checkItemValues({
    sound_select = 2
})

sendSoundSwitchTonePlayUpdated(2)
checkItemValues({
    sound_playback = sound_playback_enum.play
})

sendSoundSwitchTonePlayUpdated(3)
checkItemValues({})

sendSoundSwitchTonePlayUpdated(0)
checkItemValues({
    sound_playback = sound_playback_enum.stop
})
