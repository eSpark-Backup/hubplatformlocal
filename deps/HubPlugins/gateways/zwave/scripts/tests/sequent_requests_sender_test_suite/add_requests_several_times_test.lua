local tests = require "tests"
local zwave = require "zwave"
local timer = require "timer"

zwave.request_user_code = tests.create_mock()

local requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", "item_id", 10, 1)

requests_sender:call("add_requests", { "2", "20" })
tests.assert_eq( { requests_sender:call("get_requests") }, { { "20" }, "2" } )

requests_sender:call("add_requests", { "2", "1", "10" })
tests.assert_eq( { requests_sender:call("get_requests") }, { { "20", "1", "10" }, "2" } )

requests_sender:call("add_requests", { "10", "1", "11" })
tests.assert_eq( { requests_sender:call("get_requests") }, { { "20", "10", "1", "11" }, "2" } )

tests.assert_called(zwave.request_user_code, "check request_user_code called first time", { { 10, 1, 2 } })

-- expected responds
do
    requests_sender:call("on_report", "2")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "10", "1", "11" }, "20" } )
    tests.assert_called(zwave.request_user_code, "check request_user_code called after expected respond", { { 10, 1, 20 } })

    requests_sender:call("on_report", "20")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1", "11" }, "10" } )
    tests.assert_called(zwave.request_user_code, "check request_user_code called after expected respond", { { 10, 1, 10 } })

    requests_sender:call("on_report", "10")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "11" }, "1" } )
    tests.assert_called(zwave.request_user_code, "check request_user_code called after expected respond", { { 10, 1, 1 } })

    requests_sender:call("on_report", "1")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { }, "11" } )
    tests.assert_called(zwave.request_user_code, "check request_user_code called after expected respond", { { 10, 1, 11 } })

    requests_sender:call("add_requests", { "11", "20" })
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "20" }, "11" } )

    requests_sender:call("on_report", "11")
    tests.assert_called(zwave.request_user_code, "check request_user_code called after expected respond", { { 10, 1, 20 } })
    tests.assert_eq( { requests_sender:call("get_requests") }, { { }, "20" } )

    timer.add_time(10000)
    tests.assert_eq( { requests_sender:call("get_requests") }, { }, "check no requests after actions" )
    tests.assert_eq(timer.get_count(), 0, "check timer empty after all actions")
end

-- override postponed requests by adding main requests
do
    requests_sender:call("add_postponed_requests", { "2", "20", "3" })
    requests_sender:call("add_requests", { "2", "3", "90", "90" })

    tests.assert_eq( { requests_sender:call("get_requests") }, { { "3", "90" }, "2" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["20"] = true } )
end
