local tests = require "tests"
local zwave = require "zwave"
local timer = require "timer"

zwave.request_user_code = tests.create_mock()

local requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", "item_id", 10, 1)
requests_sender._delay = 1000

requests_sender:call("add_requests", { "2", "20", "8", "3", "1" })
requests_sender:call("add_postponed_requests", { "10", "40", "50" })

-- expected report
do
    -- current request; don't handle unexpected reports
    requests_sender:call("on_report", "2" )
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "8", "3", "1" }, "20" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["10"] = true, ["40"] = true, ["50"] = true } )

    -- current request; handle unexpected reports
    requests_sender:call("on_report", "20", true)
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "3", "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["10"] = true, ["40"] = true, ["50"] = true } )
end

-- unexpected report
do
    -- main requests; don't handle unexpected reports
    requests_sender:call("on_report", "3")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "3", "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["10"] = true, ["40"] = true, ["50"] = true } )

    -- main requests; handle unexpected reports
    requests_sender:call("on_report", "3", true)
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["10"] = true, ["40"] = true, ["50"] = true } )

    -- postponed requests; don't handle unexpected reports
    requests_sender:call("on_report", "50")
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["10"] = true, ["40"] = true, ["50"] = true } )

    -- postponed requests; handle unexpected reports
    requests_sender:call("on_report", "50", true)
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["40"] = true, ["10"] = true } )
end

-- intermediate expected report
do
    timer.add_time(900)
    requests_sender:call("on_intermediate_report", "8")

    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["40"] = true, ["10"] = true } )

    timer.add_time(100)

    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["40"] = true, ["10"] = true } )

    requests_sender:call("on_intermediate_report", "8") -- reset timer again
end

-- intermediate unexpected report
do
    timer.add_time(900)
    requests_sender:call("on_intermediate_report", "1")

    tests.assert_eq( { requests_sender:call("get_requests") }, { { "1" }, "8" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["40"] = true, ["10"] = true } )

    timer.add_time(100)

    tests.assert_eq( { requests_sender:call("get_requests") }, { { }, "1" } )
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["40"] = true, ["10"] = true } )
end
