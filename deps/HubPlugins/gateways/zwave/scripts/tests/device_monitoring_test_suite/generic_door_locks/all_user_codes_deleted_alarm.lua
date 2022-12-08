local tests = require( "tests" )
local zwave = require( "zwave" )
local timer = require( "timer" )
local core = require( "core" )

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/generic_door_lock_fixture")()

zwave.request_node_values = tests.create_mock()

timer.exists = function() return false end

local function test_impl(device_info, event_sender)
    fixture:create_and_add_zwave_node(device_info)

    fixture:add_user_code(1, "1111")
    fixture:add_user_code(2, "2222")

    core.update_item_value = tests.create_mock()
    core.remove_item_dictionary_value = tests.create_mock()

    event_sender("all_user_codes_deleted", 0x21--[[alarm_type]], 0xFF --[[alarm_level. all user codes]])

    tests.assert_called(
        core.update_item_value,
        "check update core values",
        {
            { fixture:get_item_id("user_codes"), {} },
            { fixture:get_item_id("user_code_operation"), "all_user_codes_deleted"},
            { fixture:get_item_id("user_code_operation"), "no_operation" }
        }
    )

    tests.assert_called(
        core.remove_item_dictionary_value,
        "check remove item dictionary value was not called",
        {}
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
    }
)
