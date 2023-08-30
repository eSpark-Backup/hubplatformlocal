local tests = require( "tests" )
local zwave = require( "zwave" )
local timer = require( "timer" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local core = require( "core" )
local lock_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/lock_operation_event")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/generic_door_lock_fixture")()

zwave.request_node_values = tests.create_mock()

local function test_impl(device_info, event_sender, events_params)
    fixture:create_and_add_zwave_node(device_info)
    zwave.request_door_lock_state = tests.create_mock()
    zwave.synchronize_device_time = tests.create_mock()

    timer.cancel = tests.create_mock()
    timer.set_timeout_with_id = tests.create_mock()

    core.update_item_value = tests.create_mock()

    event_sender(events_params.event_type, events_params.alarm_type, events_params.alarm_level)

    tests.assert_called(
        core.update_item_value,
        nil,
        {
            { fixture:get_item_id("lock_operation"), events_params.expected_enum},
            {
                fixture:get_item_id("user_lock_operation"),
                {
                    userId = events_params.alarm_level,
                    action = events_params.expected_action,
                }
            },
        }
    )

    tests.assert_called(zwave.request_door_lock_state, "check request door lock mode", {{ fixture.node_id, fixture.channel_0_id }})
    tests.assert_called(zwave.synchronize_device_time, "check if time synchronized", nil, events_params.should_sync_time and 1 or 0)

    local user_lock_operation_id = fixture:get_item_id("user_lock_operation")
    local notification_timer_id = "notification_value_reset_" .. user_lock_operation_id
    local request_value_timer_id = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0, class_ids.door_lock, "request_door_lock_state")._timer_id

    local user_lock_operation_id = fixture:get_item_id("user_lock_operation")
    tests.assert_called_unordered(
        timer.cancel,
        "cancel timer",
        { { notification_timer_id }, { request_value_timer_id } }
    )

    tests.assert_called(
        timer.set_timeout_with_id,
       nil,
        {{
             tests.skipped_param,
             tests.skipped_param,
             tests.skipped_param,
             {
                 node_id = fixture.node_id,
                 item_name = "user_lock_operation",
                 item_id = user_lock_operation_id,
                 value = {
                     userId = -1,
                     action = "unknown"
                 }
             }
         }}
    )
end

tests.run_with_params(
    test_impl,
    fixture:get_manufacturer_specifics(),
    {
        {
            value = function (_, alarm_type, alarm_level)
                fixture:send_alarm(alarm_type, alarm_level)
            end,
            description = "send unknown_notification_recieved"
        },
        {
            value = function (event_type, alarm_type, alarm_level)
                fixture:send_notification(event_type, alarm_type, alarm_level)
            end,
            description = "send notification_recieved"
        },
    },
    {
        {
            value = {
                event_type = "keypad_lock_operation",
                alarm_type = 0x12,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.keypad_lock,
                expected_action = "lock",
            },
            description = "keypad_lock_operation event"
        },
        {
            value = {
                event_type = "keypad_unlock_operation",
                alarm_type = 0x13,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.keypad_unlock,
                expected_action = "unlock",
            },
            description = "keypad_unlock_operation event"
        },
        {
            value = {
                event_type = "valid_user_but_outside_of_schedule_operation",
                alarm_type = 0x84,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.valid_user_but_outside_of_schedule,
                expected_action = "valid_user_but_outside_of_schedule",
                should_sync_time = true
            },
            description = "valid_user_but_outside_of_schedule_operation event"
        }
    }
)
