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

local expected_items = {
    {
        name = "sound_playback",
        value_type = "token",
        enum = { "unknown", "stop", "play" },
        value = "unknown",
        channel_id = 0,
        cc_id = class_ids.sound_switch
    },
    {
        name = "sound_select",
        value = 1,
        value_max = 30,
        value_min = 1,
        channel_id = 0,
        cc_id = class_ids.sound_switch
    },
    {
        name = "sound_volume",
        value = 100,
        value_max = 100,
        value_min = 0,
        channel_id = 0,
        cc_id = class_ids.sound_switch
    },
    {
        name = "switch",
        channel_id = 0,
        cc_id = class_ids.sound_switch
    },
    {
        name = "sounding_mode",
        value_type = "token",
        enum = { "unknown", "audible", "silent" },
        value = "unknown",
        channel_id = 0,
        cc_id = class_ids.sound_switch
    },
    {
        name = "sound_list",
        value = {
            ["1"] = { name = "01 Ding Dong", duration = 5 },
            ["2"] = { name = "02 Ding Dong Tubular", duration = 9 },
            ["3"] = { name = "03 Traditional Apartment Buzzer", duration = 10 },
            ["4"] = { name = "04 Electric Apartment Buzzer", duration = 1 },
            ["5"] = { name = "05 Westminster Chimes", duration = 12 }
        },
        channel_id = 0,
        cc_id = class_ids.sound_switch
    }
}

local expected_settings = {
    {
        value_type = "action",
        value = {
            text = "Rediscover",
            lang_tag = "rediscover_tag"
        },
        tag = "rediscover"
    },
    {
        value_type = "action",
        tag = "play_default",
        cc_id = class_ids.sound_switch,
        value = { 
            text = "Play" 
        }
    }
}

local expected_devices = {
    {
        name = "Siren",
        category = "siren",
        items = expected_items,
        settings = expected_settings,
        node_id = "9000",
        battery_powered = false,
        security = "middle"
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices, nil, true)
