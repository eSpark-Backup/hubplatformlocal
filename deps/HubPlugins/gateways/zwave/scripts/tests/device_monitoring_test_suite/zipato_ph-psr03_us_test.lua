local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")

zwave.request_node_values = tests.create_mock()

local original_update_value = core.update_item_value
core.update_item_value = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 0
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
                    id = 128,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "contact_medical_service"
                            },
                            type = "emergency_alarm",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = false,
                    version = 4
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            }
                        }
                    },
                    association_groups_count = 9,
                    id = 133,
                    version = 2
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    firmware_versions = {
                        "1.18",
                        "1.9"
                    },
                    hardware_version = 1,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.5"
                },
                {
                    id = 132,
                    version = 2
                },
                {
                    id = 89,
                    version = 1
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    firmware_ids = {
                        2077
                    },
                    id = 122,
                    version = 2
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_GENERIC_CONTROLLER",
    icon_type = "ICON_TYPE_GENERIC_REMOTE_CONTROL_AV",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 316,
    product_id = 29,
    product_type = 8,
    security = {
        "none"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_REMOTE_CONTROL_AV",
    wake_up_interval = 5400
}

fixture:add_node(node)

local function test_impl(params)
    local button_item_id = storage:call(
        "find_item_id",
        item_names.button_state,
        9000,
        148,
        0,
        tostring(params.button_number))
    assert(button_item_id)

    -- button held down
    events_handler( {
        event = "simple_av_control_set_received",
        node_id = 9000,
        class_id = 148,
        channel_id = 0,
        commands = { params.command },
        key_attribute = "held_down"
    } )

    tests.assert_called(
        core.update_item_value,
        "check button updated",
        {
            { button_item_id, { button_state = "held_down", button_number = params.button_number } }
        }
    )

    -- button released
    events_handler( {
        event = "simple_av_control_set_received",
        node_id = 9000,
        class_id = 148,
        channel_id = 0,
        commands = { params.command },
        key_attribute = "released"
    } )

    tests.assert_called(
        core.update_item_value,
        "check button updated",
        {
            { button_item_id, { button_state = "released", button_number = params.button_number } },
            { button_item_id, { button_state = "idle", button_number = params.button_number } }
        }
    )

    -- button pressed 1 time, no held down
    events_handler( {
        event = "simple_av_control_set_received",
        node_id = 9000,
        class_id = 148,
        channel_id = 0,
        commands = { params.command },
        key_attribute = "press_1_time"
    } )

    tests.assert_called(
        core.update_item_value,
        "check button not updated",
        {}
    )

    timer.add_time(510)

    tests.assert_called(
        core.update_item_value,
        "check button updated",
        {
            { button_item_id, { button_state = "press_1_time", button_number = params.button_number } },
            { button_item_id, { button_state = "idle", button_number = params.button_number } }
        }
    )

    -- button pressed 1 time, held down
    events_handler( {
        event = "simple_av_control_set_received",
        node_id = 9000,
        class_id = 148,
        channel_id = 0,
        commands = { params.command },
        key_attribute = "press_1_time"
    } )

    tests.assert_called(
        core.update_item_value,
        "check button not updated",
        {}
    )

    original_update_value(button_item_id, { button_state = "held_down", button_number = params.button_number })

    timer.add_time(510)

    tests.assert_called(
        core.update_item_value,
        "check no button updated",
        {}
    )

    original_update_value(button_item_id, { button_state = "idle", button_number = params.button_number })
end

tests.run_with_params(test_impl,
{
    {
        value = { button_number = 1, command = 3 },
        description = "button 1"
    },
    {
        value = { button_number = 2, command = 4 },
        description = "button 2"
    },
    {
        value = { button_number = 3, command = 2 },
        description = "button 3"
    },
    {
        value = { button_number = 4, command = 5 },
        description = "button 4"
    },
    {
        value = { button_number = 5, command = 19 },
        description = "button 5"
    },
    {
        value = { button_number = 6, command = 22 },
        description = "button 6"
    },
    {
        value = { button_number = 7, command = 20 },
        description = "button 7"
    },
    {
        value = { button_number = 8, command = 23 },
        description = "button 8"
    }
})
