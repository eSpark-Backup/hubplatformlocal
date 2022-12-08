local tests = require "tests"
local zwave = require "zwave"
local timer = require "timer"

zwave.request_user_code = tests.create_mock()

local requests_sender = hubplugins.object("sequent_requests_sender/user_codes", "HUB:zwave/scripts/utils", "item_id", 10, 1)

requests_sender:call("add_postponed_requests", { "2", "20", "8", "3"})
requests_sender:call("add_postponed_requests", { "3", "7", "15" })
requests_sender:call("add_requests", { "2", "9", "20", "30", "21", "22" })

tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["8"] = true, ["3"] = true, ["7"] = true, ["15"] = true } )
tests.assert_eq( { requests_sender:call("get_requests") }, { { "9", "20", "30", "21", "22" }, "2" } )

-- remove_requests
do
    requests_sender:call("remove_requests", { "20", "2", "3", "200", "22" })
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["8"] = true, ["7"] = true, ["15"] = true } )
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "9", "30", "21" }, "2" } )
end

-- remove_requests_if
do
    requests_sender:call("remove_requests_if", function(id)
            return id == "2" or id == "9" or id == "15"
        end)
    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), { ["8"] = true, ["7"] = true } )
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "30", "21" }, "2" } )
end

-- check logic is ok after removing
do
    timer.add_time(5000)

    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), nil)
    tests.assert_eq( { requests_sender:call("get_requests") }, { { "30", "21", "8", "7" }, "2" } )

    requests_sender:call("on_report", "2")
    requests_sender:call("on_report", "30")
    requests_sender:call("on_report", "21")
    requests_sender:call("on_report", "8")
    requests_sender:call("on_report", "7")

    tests.assert_eq( { requests_sender:call("get_requests") }, { } )

    tests.assert_called(zwave.request_user_code, 
        "check request user code called according to reports",
        {
            { 10, 1, 2 },
            { 10, 1, 30 },
            { 10, 1, 21 },
            { 10, 1, 8 },
            { 10, 1, 7 },
        })
end

-- remove_all_requests
do
    requests_sender:call("add_postponed_requests", { "2", "20", "8", "3"})
    requests_sender:call("add_postponed_requests", { "3", "7", "15" })
    requests_sender:call("add_requests", { "2", "9", "20", "30", "21", "22" })
    requests_sender:call("remove_all_requests")

    tests.assert_eq( requests_sender:call("get_postponed_requests_set"), nil )
    tests.assert_eq( { requests_sender:call("get_requests") }, { { }, "2" } )
end
