local tests = require( "tests" )
local zwave = require("zwave")
local timer = require("timer")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()
zwave.synchronize_device_time = tests.create_mock()

local request_door_lock_state_timer = hubplugins.object("request_value_timer",
                                                        "HUB:zwave/scripts/timer_handlers",
                                                        9000,
                                                        0,
                                                        class_ids.door_lock,
                                                        "request_door_lock_state")


local function test_impl(event_type)
    request_door_lock_state_timer:call("start")

    local node_id = 9000
    local channel_0_id = 0

    local channel_0 = {
        id = channel_0_id,
        classes = {
            { id = class_ids.door_lock },
            {
                id = class_ids.notification,
                notifications = {
                    {
                        type = "access_control",
                        event_types = { event_type }
                    },
                }
            },
        }
    }

    local node = {
        id = tostring(node_id),
        manufacturer_id = 0,
        product_type = 1,
        product_id = 2,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = node_id
    } )

    zwave.request_door_lock_state = tests.create_mock()

    events_handler( {
        event = "notification_recieved",
        node_id = node_id,
        class_id = class_ids.notification,
        channel_id = 0,
        v1_alarm_type = 0x00,
        v1_alarm_level = 0x00,
        event_type = event_type,
        value = nil,
        notification_type = "access_control"
    } )

    tests.assert_called(zwave.request_door_lock_state, "check request door lock mode", {{ node_id, channel_0_id }})
    tests.assert_eq(timer.exists(request_door_lock_state_timer._timer_id), false, "check doorlock state timer cancelled")
end -- test_impl


tests.run_with_params(
    test_impl,
    {
        {
            value = "manual_lock_operation",
            description = "manual_lock_operation event",
        },
        {
            value = "manual_unlock_operation",
            description = "manual_unlock_operation event",
        },
        {
            value = "rf_lock_operation",
            description = "rf_lock_operation event",
        },
        {
            value = "rf_unlock_operation",
            description = "rf_unlock_operation event",
        },
        {
            value = "keypad_lock_operation",
            description = "keypad_lock_operation event",
        },
        {
            value = "keypad_unlock_operation",
            description = "keypad_unlock_operation event",
        },
        {
            value = "valid_user_but_outside_of_schedule_operation",
            description = "valid_user_but_outside_of_schedule_operation event"
        },
        {
            value = "rf_not_fully_locked_operation",
            description = "rf_not_fully_locked_operation event",
        },
        {
            value = "auto_lock_locked_operation",
            description = "auto_lock_locked_operation event",
        },
        {
            value = "auto_lock_not_fully_locked_operation",
            description = "auto_lock_not_fully_locked_operation event",
        },
        {
            value = "unlock_by_rf_with_invalid_user_code",
            description = "unlock_by_rf_with_invalid_user_code event",
        },
        {
            value = "lock_by_rf_with_invalid_user_code",
            description = "lock_by_rf_with_invalid_user_code event",
        },
        {
            value = "lock_jammed",
            description = "lock_jammed event",
        },
    }
)
