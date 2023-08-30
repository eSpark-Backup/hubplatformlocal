local tests = require( "tests" )
local zwave = require( "zwave" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local core = require( "core" )
local lock_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/lock_operation_event")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/generic_door_lock_fixture")()

zwave.request_node_values = tests.create_mock()

local function test_impl(device_info, event_sender, events_params)
    fixture:create_and_add_zwave_node(device_info)

    core.update_item_value = tests.create_mock()
    zwave.request_door_lock_state = tests.create_mock()

    event_sender(events_params.event_type, events_params.alarm_type, events_params.alarm_level)

    tests.assert_called(
        core.update_item_value,
        nil,
        {
            { fixture:get_item_id("lock_operation"), events_params.expected_enum}
        }
    )

    tests.assert_called(zwave.request_door_lock_state, "check request door lock mode", {{ fixture.node_id, fixture.channel_0_id }})
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
                event_type = "manual_lock_operation",
                alarm_type = 0x15,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.manual_lock,
            },
            description = "Manual lock operation (by key cylinder or inside thumb-turn)"
        },
        {
            value = {
                event_type = "manual_lock_operation",
                alarm_type = 0x15,
                alarm_level = 0x02,
                expected_enum = lock_operation_enum.manual_lock,
            },
            description = "Manual lock operation (by touch function (lock and leave))"
        },
        {
            value = {
                event_type = "manual_lock_operation",
                alarm_type = 0x15,
                alarm_level = 0x03,
                expected_enum = lock_operation_enum.manual_lock,
            },
            description = "Manual lock operation (By inside button)"
        },
        {
            value = {
                event_type = "manual_unlock_operation",
                alarm_type = 0x16,
                alarm_level = 0x00,
                expected_enum = lock_operation_enum.manual_unlock,
            },
            description = "Manual unlock operation (By key cylinder or inside thumb turn)"
        },
        {
            value = {
                event_type = "manual_not_fully_locked_operation",
                alarm_type = 0x11,
                alarm_level = 0x00,
                expected_enum = lock_operation_enum.manual_not_fully_locked,
            },
            description = "Manual not fully locked operation"
        },
        {
            value = {
                event_type = "rf_not_fully_locked_operation",
                alarm_type = 0x17,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.rf_not_fully_locked,
            },
            description = "RF not fully locked operation"
        },
        {
            value = {
                event_type = "rf_lock_operation",
                alarm_type = 0x18,
                alarm_level = 0x00,
                expected_enum = lock_operation_enum.rf_lock,
            },
            description = "RF lock operation"
        },
        {
            value = {
                event_type = "rf_unlock_operation",
                alarm_type = 0x19,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.rf_unlock,
            },
            description = "RF unlock operation"
        },
        {
            value = {
                event_type = "auto_lock_not_fully_locked_operation",
                alarm_type = 0x1A,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.auto_lock_not_fully_locked,
            },
            description = "Auto lock not fully locked operation"
        },
        {
            value = {
                event_type = "auto_lock_locked_operation",
                alarm_type = 0x1B,
                alarm_level = 0x01,
                expected_enum = lock_operation_enum.auto_lock_locked,
            },
            description = "Auto lock locked operation"
        },
    }
)
