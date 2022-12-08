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
    {
        duration = 5,
        tone_id = 1,
        tone_name = "01 Ding Dong"
    },
    {
        duration = 9,
        tone_id = 2,
        tone_name = "02 Ding Dong Tubular"
    },
    {
        duration = 10,
        tone_id = 3,
        tone_name = "03 Traditional Apartment Buzzer"
    },
    {
        duration = 1,
        tone_id = 4,
        tone_name = "04 Electric Apartment Buzzer"
    },
    {
        duration = 12,
        tone_id = 5,
        tone_name = "05 Westminster Chimes"
    },
    {
        duration = 7,
        tone_id = 6,
        tone_name = "06 Chimes"
    },
    {
        duration = 31,
        tone_id = 7,
        tone_name = "07 Cuckoo"
    },
    {
        duration = 6,
        tone_id = 8,
        tone_name = "08 Traditional Bell"
    },
    {
        duration = 11,
        tone_id = 9,
        tone_name = "09 Smoke Alarm 1"
    },
    {
        duration = 5,
        tone_id = 10,
        tone_name = "10 Smoke Alarm 2"
    },
    {
        duration = 35,
        tone_id = 11,
        tone_name = "11 Fire Evacuation Buzzer"
    },
    {
        duration = 4,
        tone_id = 12,
        tone_name = "12 CO Sensor"
    },
    {
        duration = 6,
        tone_id = 13,
        tone_name = "13 Klaxon"
    },
    {
        duration = 40,
        tone_id = 14,
        tone_name = "14 Deep Klaxon"
    },
    {
        duration = 37,
        tone_id = 15,
        tone_name = "15 Warning Tone"
    },
    {
        duration = 45,
        tone_id = 16,
        tone_name = "16 Tornado Siren"
    },
    {
        duration = 35,
        tone_id = 17,
        tone_name = "17 Alarm"
    },
    {
        duration = 62,
        tone_id = 18,
        tone_name = "18 Deep Alarm"
    },
    {
        duration = 15,
        tone_id = 19,
        tone_name = "19 Alarm Archangel"
    },
    {
        duration = 7,
        tone_id = 20,
        tone_name = "20 Alarm Shrill"
    },
    {
        duration = 8,
        tone_id = 21,
        tone_name = "21 Digital Siren"
    },
    {
        duration = 63,
        tone_id = 22,
        tone_name = "22 Alert Series"
    },
    {
        duration = 3,
        tone_id = 23,
        tone_name = "23 Ship Bell"
    },
    {
        duration = 9,
        tone_id = 24,
        tone_name = "24 Clock Buzzer"
    },
    {
        duration = 3,
        tone_id = 25,
        tone_name = "25 Christmas Tree"
    },
    {
        duration = 11,
        tone_id = 26,
        tone_name = "26 Gong"
    },
    {
        duration = 0,
        tone_id = 27,
        tone_name = "27 Single Bell Ting"
    },
    {
        duration = 11,
        tone_id = 28,
        tone_name = "28 Tonal Pulse"
    },
    {
        duration = 2,
        tone_id = 29,
        tone_name = "29 Upwards Tone"
    },
    {
        duration = 27,
        tone_id = 30,
        tone_name = "30 Door Open"
    }
}

local tonesList = {}
for _, tone_info in pairs(tones) do
    tonesList[tostring(tone_info.tone_id)] = {
        name = tone_info.tone_name,
        duration = tone_info.duration
    }
end

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.5"
                    },
                    hardware_version = 162,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "5.3"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                {
                                    endpoint_id = 0,
                                    node_id = 1
                                }
                            }
                        }
                    },
                    association_groups_count = 9,
                    id = 142,
                    version = 3
                },
                {
                    id = 96,
                    version = 4
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                },
                {
                    firmware_ids = {
                        1186
                    },
                    id = 122,
                    version = 4
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "product_moved"
                            },
                            type = "home_security",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "replace_battery_soon"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                }
            },
            id = 0
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 1
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 2
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "replace_battery_soon"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 3
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "replace_battery_soon"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 4
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "replace_battery_soon"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 5
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 6
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 7
        },
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    association_groups_count = 0,
                    id = 142,
                    version = 3
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "siren_active"
                            },
                            type = "siren",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 8
                },
                {
                    id = 121,
                    tones = tones,
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 159,
                    version = 1
                }
            },
            id = 8
        }
    },
    generic_type = "GENERIC_TYPE_AV_CONTROL_POINT",
    icon_type = "ICON_TYPE_GENERIC_SOUND_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 881,
    product_id = 162,
    product_type = 259,
    security = {
        "authenticated"
    },
    security_class_started = {
        "authenticated"
    },
    specific_type = "SPECIFIC_TYPE_SOUND_SWITCH"
}

zwave.add_node( node )

events_handler( {
    event = "node_added",
    node_id = 9000
} )

local function getItems(channel_id)
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
        },
        {
            name = "siren_alarm",
            tag = "siren",
            value_type = "token",
            enum = { "unknown", "siren_inactive", "siren_active" },
            value = "siren_inactive",
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
            cc_id = 0x71,
            is_merged = false
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

local function makeDevice(name, items, subdevices)
    return {
        name = name,
        category = "siren",
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
        "Doorbell 6",
        concat(getItems(0), getHomeSecurityItems(0), getPowerManagementItems(0)),
        {
            makeSubdevice("Doorbell 6 Channel 1", getItems(1)),
            makeSubdevice("Doorbell 6 Channel 2", getItems(2)),
            makeSubdevice("Doorbell 6 Channel 3", getItems(3), getPowerManagementItems(3)),
            makeSubdevice("Doorbell 6 Channel 4", getItems(4), getPowerManagementItems(4)),
            makeSubdevice("Doorbell 6 Channel 5", getItems(5), getPowerManagementItems(5)),
            makeSubdevice("Doorbell 6 Channel 6", getItems(6)),
            makeSubdevice("Doorbell 6 Channel 7", getItems(7)),
            makeSubdevice("Doorbell 6 Channel 8", getItems(8))
        }
    )
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices)