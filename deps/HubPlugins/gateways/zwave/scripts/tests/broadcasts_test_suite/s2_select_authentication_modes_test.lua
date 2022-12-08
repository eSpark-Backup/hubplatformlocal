local tests = require "tests"
local core = require "core"
local timer = require "timer"
local zwave = require "zwave"
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))

core.send_ui_broadcast = tests.create_mock()
zwave.set_auth_mode = tests.create_mock()
zwave.stop_include = tests.create_mock()

local modes = {"s0"}

local function check_broadcast_sent()
    tests.assert_called(core.send_ui_broadcast, "check if broadcast sent", {{{event = "s2_select_authentication_modes", modes = modes}}})
end

local function check_no_broadcast_sent()
    tests.assert_called(core.send_ui_broadcast, "check if no broadcast sent", {})
end

-- test no cancel
do
    -- init broadcasting
    events_handler({event = "s2_select_authentication_modes", modes = modes})
    check_broadcast_sent()

    tests.assert_eq(timer.exists("time_id_broadcast_s2_select_authentication_modes"), true)

    for i = 1, 5 do
        timer.add_time(10000)
        check_broadcast_sent()
    end
    tests.assert_eq(timer.exists("time_id_broadcast_s2_select_authentication_modes"), false)

    timer.add_time(10000)
    check_no_broadcast_sent()
end

local function test_canceled(action_to_cancel)
    core.send_ui_broadcast = tests.create_mock()

    -- init broadcasting
    events_handler({event = "s2_select_authentication_modes", modes = modes})
    check_broadcast_sent()

    tests.assert_eq(timer.exists("time_id_broadcast_s2_select_authentication_modes"), true)

    for i = 1, 4 do
        timer.add_time(10000)
        check_broadcast_sent()
    end

    tests.assert_eq(timer.exists("time_id_broadcast_s2_select_authentication_modes"), true)

    action_to_cancel()

    tests.assert_eq(timer.exists("time_id_broadcast_s2_select_authentication_modes"), false)
end

test_canceled(function() loadfile("HUB:zwave/scripts/set_authentication_mode")({}) end)
test_canceled(function() loadfile("HUB:zwave/scripts/stop_include")() end)
test_canceled(function() events_handler({event = "include_finished"}) end)
test_canceled(function() events_handler({event = "include_finished_timeout"}) end)
test_canceled(function() events_handler({event = "include_finished_error"}) end)
