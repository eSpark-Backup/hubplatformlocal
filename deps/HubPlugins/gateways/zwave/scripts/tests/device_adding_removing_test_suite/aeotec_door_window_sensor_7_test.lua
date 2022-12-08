local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl(params)
    local node = {
        id = "9000",
        channels = {
            {
                classes = {
                    { id = 94,  version = 2 },
                    { id = 133, version = 2 },
                    { id = 142, version = 3 },
                    { id = 89,  version = 1 },
                    { id = 134, version = 3 },
                    { id = 114, version = 2 },
                    { id = 112, version = 1 },
                    { id = 132, version = 2 },
                    { id = 128, version = 1 },
                    {
                        buttons = {
                            {
                                button_attributes = {
                                    "press_1_time",
                                    "released",
                                    "held_down",
                                    "press_2_times",
                                    "press_3_times",
                                    "press_4_times",
                                    "press_5_times"
                                },
                                button_id = 1
                            }
                        },
                        id = 91,
                        version = 3
                    },
                    { id = 48, sensor_types = { 11 }, version = 2 },
                    {
                        id = 113,
                        notifications = {
                            {
                                event_types = { "dw_is_opened","dw_is_closed" },
                                type = "access_control"
                            },
                            {
                                event_types = { "product_cover_removed" },
                                type = "home_security"
                            }
                        },
                        v1_supported = false,
                        version = 8
                    }
                },
                id = 0
            }
        },
        generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
        specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

        is_battery_powered = true,

        manufacturer_id = params.manufacturer_id,
        product_type = params.product_type,
        product_id = params.product_id,

        security = { "none" },
        security_class_started = { "unauthenticated" },
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )


    -- check adding result
    local expected_devices = {
        {
            name = params.device_name,
            category = "security_sensor",
            subcategory = "door",
            items = {
                {
                    name = "button_state",
                    value_type = "button_state",
                    enum = {
                        "press_1_time",
                        "press_2_times",
                        "press_3_times",
                        "press_4_times",
                        "press_5_times",
                        "released",
                        "held_down",
                        "idle"
                    },
                    tag = "1",
                    value = {button_number = 1, button_state = "idle"},
                    cc_id = class_ids.central_scene,
                    channel_id = 0
                },
                {
                    name = "tampering_cover_alarm",
                    cc_id = class_ids.notification,
                    tag = "home_security",
                    channel_id = 0,
                },
                {
                    name = "battery",
                    cc_id = class_ids.battery,
                    channel_id = 0,
                },
                {
                    name = "dw_state",
                    cc_id = class_ids.notification,
                    tag = "access_control",
                    channel_id = 0,
                },
                {
                    name = "security_threat",
                    tag = "access_control.security_tag",
                    cc_id = class_ids.notification,
                    channel_id = 0,
                    main_item_name = "dw_state"
                }
            },
            node_id = "9000",
            battery_powered = true,
            security = "authenticated",

            subdevices = {
                {
                    name = "Door/Window Tilt Sensor",
                    category = "security_sensor",
                    subcategory = "tilt",
                    items = {
                        {
                            name = "tilt",
                            cc_id = class_ids.sensor_binary,
                            tag = "access_control",
                            channel_id = 0,
                        },
                        {
                            name = "security_threat",
                            tag = "access_control.security_tag",
                            cc_id = class_ids.sensor_binary,
                            channel_id = 0,
                            main_item_name = "tilt"
                        },
                    }
                }
            }
        }
    }

    local check_context = {}

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = params.manufacturer_id,
        product_type = params.product_type,
        product_id = params.product_id,
    } )

    -- check node removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                manufacturer_id = 881,
                product_type = 258,
                product_id = 7,
                device_name = "Door/Window Sensor 7"
            },
            description = "test with 881_258_7"
        },
        {
            value = {
                manufacturer_id = 881,
                product_type = 0,
                product_id = 7,
                device_name = "Door/Window Sensor 7"
            },
            description = "test with 881_0_7"
        },
        {
            value = {
                manufacturer_id = 881,
                product_type = 258,
                product_id = 12,
                device_name = "Door/Window Sensor 7 Pro"
            },
            description = "test with 881_258_12"
        },
        {
            value = {
                manufacturer_id = 881,
                product_type = 2,
                product_id = 12,
                device_name = "Door/Window Sensor 7 Pro"
            },
            description = "test with 881_2_12"
        }
    }
)
