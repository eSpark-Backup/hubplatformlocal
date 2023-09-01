local tests = require( "tests" )
local zwave = require("zwave")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

zwave.request_node_values = tests.create_mock()

local tones = {
    {duration = 5, tone_id = 1, tone_name = "01 Ding Dong"},
    {duration = 9, tone_id = 2, tone_name = "02 Ding Dong Tubular"},
    {duration = 10, tone_id = 3, tone_name = "03 Traditional Apartment Buzzer"},
    {duration = 1, tone_id = 4, tone_name = "04 Electric Apartment Buzzer"},
    {duration = 12, tone_id = 5, tone_name = "05 Westminster Chimes"},
}
local tonesList = {}
for _, tone_info in pairs(tones) do
    tonesList[tostring(tone_info.tone_id)] = {
        name = tone_info.tone_name,
        duration = tone_info.duration
    }
end

local function test_impl(params)
    local channel_0 = {
        classes = params.classes,    
        id = 0,
    }
    
    local node = {
        id = "9000",
        manufacturer_id = 0,
        product_type = 2,
        product_id = 3,
        is_battery_powered = true,
    
        specific_type = "SPECIFIC_TYPE_SOUND_SWITCH",
        generic_type = "GENERIC_TYPE_STATIC_CONTROLLER",
        
        icon_type = "ICON_TYPE_GENERIC_CENTRAL_CONTROLLER",
    
        security = {"none"},
        security_class_started = {"none"},
     
        channels = { channel_0 }
    }
    
    local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

    local zwave = require( "zwave" )
    zwave.request_node_values = tests.create_mock()
    
    -- prepare zwave
    zwave.add_node( node )
    
    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000 
    } )
    
    
    local expected_devices = {
        {
            name = params.device_name,
            category = params.category,
            items = params.items,
            node_id = "9000",
            battery_powered = true,
        }
    }
    
    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {}
    }
    
    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
    
    loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
    } )

    -- check removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params( 
    test_impl, 
    {
        {
            value = {
                device_name = "Dorrbell",
                category = "doorbell",
                items = {
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
                        name = "sounding_mode",
                        value_type = "token",
                        enum = { "unknown", "audible", "silent" },
                        value = "unknown",
                        channel_id = 0,
                        cc_id = class_ids.sound_switch
                    },
                    {
                        name = "sound_list",
                        value = tonesList,
                        channel_id = 0,
                        cc_id = class_ids.sound_switch
                    },
                    {
                        name = "switch",
                        channel_id = 0,
                        cc_id = class_ids.sound_switch
                    },
                    {
                        name = "battery",
                        cc_id = class_ids.battery,
                        channel_id = 0,
                    },
                },
                classes = {
                    {
                        id = class_ids.sound_switch,
                        tones = tones,
                        version = 1            
                    },
                    {
                        id = class_ids.battery,
                        version = 2
                    }
                }                
            },
            description = "test of detection device as doorbell"
        },
        {
            value = {
                device_name = "Siren",
                category = "siren",
                items = {
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
                        name = "sounding_mode",
                        value_type = "token",
                        enum = { "unknown", "audible", "silent" },
                        value = "unknown",
                        channel_id = 0,
                        cc_id = class_ids.sound_switch
                    },
                    {
                        name = "sound_list",
                        value = tonesList,
                        channel_id = 0,
                        cc_id = class_ids.sound_switch
                    },
                    {
                        name = "switch",
                        channel_id = 0,
                        cc_id = class_ids.sound_switch
                    },
                    {
                        name = "siren_alarm",
                        value_type = "token",
                        tag = "siren",
                        enum = { "unknown", "siren_inactive", "siren_active" },
                        value = "siren_inactive",
                        channel_id = 0,
                        cc_id = class_ids.notification
                    },
                    {
                        name = "battery",
                        cc_id = class_ids.battery,
                        channel_id = 0,
                    }
                },
                classes = {
                    --because of this CC, we can detect this device as a siren: 
                    {
                        id = class_ids.notification,
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
                        id = class_ids.sound_switch,
                        tones = tones,
                        version = 1            
                    },
                    {
                        id = class_ids.battery,
                        version = 2
                    }
                }
            },
            description = "test of detection device as siren"
        }
    }
)
