local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require( "core" )

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/generic_door_lock_fixture")()

zwave.request_node_values = tests.create_mock()

local function test_impl(device_info, event_sender, events_params)
    fixture:create_and_add_zwave_node(device_info)

    core.update_item_value = tests.create_mock()
    zwave.request_user_code = tests.create_mock()

    event_sender(events_params.event_type, events_params.alarm_type, events_params.alarm_level)

    tests.assert_called(
        core.update_item_value,
        nil,
        {
            { fixture:get_item_id("user_code_operation"), events_params.expected_enum },
            { fixture:get_item_id("user_code_operation"), "no_operation" }
        }
    )

    tests.assert_called(
        zwave.request_user_code,
        "request actual state of user code",
        {{ fixture.node_id, fixture.channel_0_id, events_params.alarm_level--[[user_id]] }}
    )

    local user_code_requests_sender = hubplugins.object("sequent_requests_sender/user_codes", 
                                                        "HUB:zwave/scripts/utils", 
                                                        fixture:get_item_id("user_codes"), 
                                                        fixture.node_id, 
                                                        fixture.channel_0_id)
    tests.assert_eq({ user_code_requests_sender:call("get_requests") }, { {}, tostring(events_params.alarm_level) }, "check user code requests")
    user_code_requests_sender:call("remove_all_requests")
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
                event_type = "single_user_code_deleted",
                alarm_type = 0x21,
                alarm_level = 0x01,
                expected_enum = "single_user_code_deleted",
            },
            description = "single_user_code_deleted event"
        },
        {
            value = {
                event_type = "new_user_code_added",
                alarm_type = 0x70,
                alarm_level = 0x01,
                expected_enum = "new_user_code_added",
            },
            description = "new_user_code_added event"
        },
        {
            value = {
                event_type = "new_user_code_not_added_due_to_duplicated_code",
                alarm_type = 0x71,
                alarm_level = 0x01,
                expected_enum = "new_user_code_not_added_due_to_duplicated_code",
            },
            description = "new_user_code_not_added_due_to_duplicated_code event"
        },
        {
            value = {
                event_type = "new_user_code_not_added_due_to_duplicated_code",
                alarm_type = 0x22,
                alarm_level = 0x01,
                expected_enum = "new_user_code_not_added_due_to_duplicated_code",
            },
            description = "new_user_code_not_added_due_to_duplicated_code event 2"
        },
    }
)
