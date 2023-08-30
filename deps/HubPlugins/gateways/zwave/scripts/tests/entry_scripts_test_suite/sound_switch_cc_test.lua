local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local sounding_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")
local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )
local set_setting_value = loadfile( "HUB:zwave/scripts/set_setting_value" )

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

local sound_select_item_id = storage:call("find_item_id", "sound_select", 9000, class_ids.sound_switch, 0 )
local sound_playback_item_id = storage:call("find_item_id", "sound_playback", 9000, class_ids.sound_switch, 0 )
local play_default_setting_id = storage:call("find_setting_id", 9000, class_ids.sound_switch, "play_default" )

assert(sound_select_item_id, "No sound_select item found")
assert(sound_playback_item_id, "No sound_playback item found")
assert(play_default_setting_id, "No play_default setting found")

core.update_item_value(sound_select_item_id, 3);

-- Check item value set
zwave.set_sound_switch_tone_play = tests.create_mock()
zwave.request_sound_switch_tone_play = tests.create_mock()

set_item_value({item_id = sound_playback_item_id, value = "play"})

tests.assert_called(zwave.set_sound_switch_tone_play, "check sound played", { { "9000", 0, 3, 0 } })
tests.assert_called(zwave.request_sound_switch_tone_play, "check sound played", { { "9000", 0 } })

-- Check setting set
zwave.set_sound_switch_tone_play = tests.create_mock()
zwave.request_sound_switch_tone_play = tests.create_mock()

set_setting_value({setting_id = play_default_setting_id})

tests.assert_called(zwave.set_sound_switch_tone_play, "check sound played", { { "9000", 0, 0xFF, 0 } })
tests.assert_called(zwave.request_sound_switch_tone_play, "check sound played", { { "9000", 0 } })
