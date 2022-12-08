local tests = require "tests"
local zwave = require "zwave"
local timer = require "timer"

zwave.request_user_code = tests.create_mock()

local requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", "item_id", 10, 1)

requests_sender:call("add_requests", { "2", "20", "8", "3" })
tests.assert_eq( { requests_sender:call("get_requests") }, { { "20", "8", "3" }, "2" } )

tests.assert_called(zwave.request_user_code, "check request_user_code called after first call", { { 10, 1, 2 } })

-- expected respond
do
    requests_sender:call("on_report", "2")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "8", "3" }, "20" } )

    tests.assert_called(zwave.request_user_code, "check request_user_code called after expected respond", { { 10, 1, 20 } })
end

-- unexpected expected respond
do
    requests_sender:call("on_report", "2", true)
    requests_sender:call("on_report", "3", true)
    requests_sender:call("on_report", "4", true)

    tests.assert_eq( { requests_sender:call("get_requests") }, { { "8" }, "20" } )
    tests.assert_called(zwave.request_user_code, "check request_user_code not called after unexpected", { })
end

-- timeout
do
    timer.add_time(10000 - 1)
    tests.assert_called(zwave.request_user_code, "check request_user_code not called after not enough timeout", { })

    timer.add_time(1)
    tests.assert_called(zwave.request_user_code, "check request_user_code called after enough timeout", { { 10, 1, 8 } })

    tests.assert_eq( { requests_sender:call("get_requests") }, { { }, "8" } )

    tests.assert_eq(timer.get_count(), 1)

    timer.add_time(10000)
    tests.assert_called(zwave.request_user_code, "check request_user_code not called after last timeout", { })
    tests.assert_eq(timer.get_count(), 0, "check timer empty after all actions")

    tests.assert_eq( { requests_sender:call("get_requests") }, { }, "check no requests in queue" )
end
