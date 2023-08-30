local tests = require "tests"
local zwave = require "zwave"
local timer = require "timer"

zwave.request_user_code = tests.create_mock()

local requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", "item_id", 10, 1)

requests_sender:call("add_postponed_requests", { "2", "20", "8", "3" })
tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["2"] = true, ["3"] = true, ["8"] = true, ["20"] = true } )
tests.assert_eq( { requests_sender:call("get_requests") }, { } )
tests.assert_eq(timer.get_count(), 2, "check two timers are active: base postponed timer and first postponed")

timer.add_time(1000)

requests_sender:call("add_postponed_requests", { "8", "20", "1", "4" })
tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["2"] = true, ["3"] = true, ["8"] = true, ["20"] = true, ["1"] = true, ["4"] = true } )
tests.assert_eq( { requests_sender:call("get_requests") }, { } )
tests.assert_eq(timer.get_count(), 3, "check three timers are active: base postponed timer, first postponed and second postponed")

timer.add_time(5000 - 1000 - 1)
tests.assert_eq( { requests_sender:call("get_requests") }, { } )


timer.add_time(1)
tests.assert_eq( { requests_sender:call("get_requests") }, { { "3" }, "2" } )
tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["8"] = true, ["20"] = true, ["1"] = true, ["4"] = true } )
tests.assert_eq(timer.get_count(), 3, "check three timers are active: base postponed timer, second postponed and main timer")

timer.add_time(1000)
tests.assert_eq( { requests_sender:call("get_requests") }, { { "3", "8", "20", "1", "4" }, "2" } )
tests.assert_eq( requests_sender:call("get_postponed_requests_set"), nil )
tests.assert_eq(timer.get_count(), 1, "check only main request timer is active")
