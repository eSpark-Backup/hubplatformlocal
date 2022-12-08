local tests = require("tests")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))
local zwave = require("zwave")

zwave.request_node_values = tests.create_mock()

local function concat(...)
    local result = {}

    for _, items in ipairs({ ... }) do
        for _, item in ipairs(items) do
            table.insert(result, item)
        end
    end

    return result
end

local tones = {
    { tone_id = 1 , duration = 5,  tone_name = "01 Ding Dong" },
    { tone_id = 2 , duration = 9,  tone_name = "02 Ding Dong Tubular" },
    { tone_id = 3 , duration = 10, tone_name = "03 Traditional Apartment Buzzer" },
    { tone_id = 4 , duration = 1,  tone_name = "04 Electric Apartment Buzzer" },
    { tone_id = 5 , duration = 12, tone_name = "05 Westminster Chimes" },
    { tone_id = 6 , duration = 7,  tone_name = "06 Chimes" },
    { tone_id = 7 , duration = 31, tone_name = "07 Cuckoo" },
    { tone_id = 8 , duration = 6,  tone_name = "08 Traditional Bell" },
    { tone_id = 9 , duration = 11, tone_name = "09 Smoke Alarm 1" },
    { tone_id = 10, duration = 5,  tone_name = "10 Smoke Alarm 2" },
    { tone_id = 11, duration = 35, tone_name = "11 Fire Evacuation Buzzer" },
    { tone_id = 12, duration = 4,  tone_name = "12 CO Sensor" },
    { tone_id = 13, duration = 6,  tone_name = "13 Klaxon" },
    { tone_id = 14, duration = 40, tone_name = "14 Deep Klaxon" },
    { tone_id = 15, duration = 37, tone_name = "15 Warning Tone" },
    { tone_id = 16, duration = 45, tone_name = "16 Tornado Siren" },
    { tone_id = 17, duration = 35, tone_name = "17 Alarm" },
    { tone_id = 18, duration = 62, tone_name = "18 Deep Alarm" },
    { tone_id = 19, duration = 15, tone_name = "19 Alarm Archangel" },
    { tone_id = 20, duration = 7,  tone_name = "20 Alarm Shrill" },
    { tone_id = 21, duration = 8,  tone_name = "21 Digital Siren" },
    { tone_id = 22, duration = 63, tone_name = "22 Alert Series" },
    { tone_id = 23, duration = 3,  tone_name = "23 Ship Bell" },
    { tone_id = 24, duration = 9,  tone_name = "24 Clock Buzzer" },
    { tone_id = 25, duration = 3,  tone_name = "25 Christmas Tree" },
    { tone_id = 26, duration = 11, tone_name = "26 Gong" },
    { tone_id = 27, duration = 0,  tone_name = "27 Single Bell Ting" },
    { tone_id = 28, duration = 11, tone_name = "28 Tonal Pulse" },
    { tone_id = 29, duration = 2,  tone_name = "29 Upwards Tone" },
    { tone_id = 30, duration = 27, tone_name = "30 Door Open" }
}

local tonesList = {}
for _, tone_info in pairs(tones) do
    tonesList[tostring(tone_info.tone_id)] = {
        name = tone_info.tone_name,
        duration = tone_info.duration
    }
end

local notifications = {
    home_security = {
        type = "home_security",
        event_types = { "product_moved" }
    },
    power_management = {
        type = "power_management",
        event_types = { "replace_battery_soon" }
    },
    siren = {
        type = "siren",
        event_types = { "siren_active" }
    }
}

local function makeChannel(channel_id, class_ids, customClasses)
    local classes = {}

    for idx, class_id in ipairs(class_ids) do
        if (customClasses[class_id]) then
            classes[idx] = customClasses[class_id]
            classes[idx].id = class_id
        else
            classes[idx] = { id = class_id }
        end
    end

    return {
        id = channel_id,
        classes = classes
    }
end

local function makeEndpointChannel(channel_id, notifications)
    return makeChannel(
        channel_id,
        { 0x5E, 0x85, 0x59, 0x8E, 0x71, 0x79 },
        {
            [0x71] = {
                notifications = notifications
            },
            [0x79] = {
                tones = tones
            }
        }
    )
end

local node = {
    id = "9000",
    manufacturer_id = 881,
    product_type = 3,
    product_id = 164,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_SOUND_SWITCH",
    generic_type = "GENERIC_TYPE_AV_CONTROL_POINT",

    security = { "none" },
    security_class_started = { "none" },

    channels = {
        makeChannel(
            0,
            { 0x5E, 0x86, --[[ 0x70, ]] 0x72, 0x59, 0x85, 0x8E, 0x60, 0x71, 0x79 },
            {
                [0x71] = {
                    notifications = {
                        notifications.home_security,
                        notifications.power_management,
                        notifications.siren
                    }
                },
                [0x79] = {
                    tones = tones
                }
            }
        ),
        makeEndpointChannel(1, { notifications.siren }),
        makeEndpointChannel(2, { notifications.siren }),
        makeEndpointChannel(3, { notifications.siren, notifications.power_management }),
        makeEndpointChannel(4, { notifications.siren, notifications.power_management }),
        makeEndpointChannel(5, { notifications.siren, notifications.power_management }),
        makeEndpointChannel(6, { notifications.siren }),
        makeEndpointChannel(7, { notifications.siren }),
        makeEndpointChannel(8, { notifications.siren })
    }
}

zwave.add_node( node )

events_handler( {
    event = "node_added",
    node_id = 9000
} )

local function getSoundSwitchItems(channel_id)
    return {
        {
            name = "sound_playback",
            value_type = "token",
            enum = { "unknown", "stop", "play" },
            value = "unknown",
            channel_id = channel_id,
            cc_id = class_ids.sound_switch
        },
        {
            name = "sound_select",
            value = 1,
            value_max = 30,
            value_min = 1,
            channel_id = channel_id,
            cc_id = class_ids.sound_switch
        },
        {
            name = "sound_volume",
            value = 100,
            value_max = 100,
            value_min = 0,
            channel_id = channel_id,
            cc_id = class_ids.sound_switch
        },
        {
            name = "sounding_mode",
            value_type = "token",
            enum = { "unknown", "audible", "silent" },
            value = "unknown",
            channel_id = channel_id,
            cc_id = class_ids.sound_switch
        },
        {
            name = "sound_list",
            value = tonesList,
            channel_id = channel_id,
            cc_id = class_ids.sound_switch
        },
        {
            name = "switch",
            channel_id = channel_id,
            cc_id = class_ids.sound_switch
        }
    }
end

local function getSirenItems(channel_id)
    return {
        {
            name = "siren_alarm",
            value_type = "token",
            tag = "siren",
            enum = { "unknown", "siren_inactive", "siren_active" },
            value = "siren_inactive",
            channel_id = channel_id,
            cc_id = 0x71
        }
    }
end

local function getPowerManagementItems(channel_id)
    return {
        {
            name = "battery_maintenance_state",
            tag = "power_management",
            value_type = "token",
            enum = { "unknown", "battery_maintenance_ok", "replace_battery_soon" },
            value = "unknown",
            channel_id = channel_id,
            cc_id = 0x71
        }
    }
end

local function getHomeSecurityItems(channel_id)
    return {
        {
            name = "tampering_move_alarm",
            tag = "home_security",
            value_type = "token",
            enum = { "unknown", "no_moving", "product_moved" },
            value = "unknown",
            channel_id = channel_id,
            cc_id = 0x71
        }
    }
end

local function makeDevice(name, items, subdevices)
    return {
        name = name,
        category = "doorbell",
        items = items,
        node_id = "9000",
        battery_powered = false,
        security = "no",

        subdevices = subdevices
    }
end

local function makeSubdevice(name, ...)
    return makeDevice(name, concat(...), {})
end

local expected_devices = {
    makeDevice(
        "Door Bell",
        concat(getSoundSwitchItems(0), getSirenItems(0), getPowerManagementItems(0), getHomeSecurityItems(0)),
        {
            makeSubdevice("Door Bell 1", getSoundSwitchItems(1), getSirenItems(1)),
            makeSubdevice("Door Bell 2", getSoundSwitchItems(2), getSirenItems(2)),
            makeSubdevice("Door Bell 3", getSoundSwitchItems(3), getSirenItems(3), getPowerManagementItems(3)),
            makeSubdevice("Door Bell 4", getSoundSwitchItems(4), getSirenItems(4), getPowerManagementItems(4)),
            makeSubdevice("Door Bell 5", getSoundSwitchItems(5), getSirenItems(5), getPowerManagementItems(5)),
            makeSubdevice("Door Bell 6", getSoundSwitchItems(6), getSirenItems(6)),
            makeSubdevice("Door Bell 7", getSoundSwitchItems(7), getSirenItems(7)),
            makeSubdevice("Door Bell 8", getSoundSwitchItems(8), getSirenItems(8))
        }
    )
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices)
