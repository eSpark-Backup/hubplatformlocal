local fixture = {}

local timer = require("timer")
local tests = require("tests")
local core = require("core")
local storage = require("storage")

function fixture.init()
    timer.exists = function(...) return false end

    timer.set_timeout_with_id = tests.create_mock()
    timer.cancel = tests.create_mock()
end

function fixture.add_device(model, serial)
    loadfile("HUB:2gig/scripts/add_device")(
        {
            model = model,
            serial = serial
        }
    )
end

function fixture.set_local_time(time)
    core.get_local_time = function(...) return time end
end

function fixture.check_item_value(item_id, value)
    tests.assert_eq(
        core.get_item(item_id).value, 
        value, 
        "check core item value"
    )
end

function fixture.check_event_table(value)
    tests.assert_eq(storage.get_table("2gig_pin_code_restriction_nearest_event_time"), value)
end

function fixture.check_timer(inited, timeout)
    if inited then
        tests.assert_called(
            timer.set_timeout_with_id, 
            "Check timer creation call", 
            {
                {
                    timeout, 
                    "2gig_pin_codes_restrictions_timer", 
                    "HUB:2gig/scripts/helpers/pin_codes_restrictions/timer/handler", 
                    {}
                }
            }
        )
        tests.assert_called(timer.cancel, "Check timer cancel call", {})
    else
        tests.assert_called(timer.set_timeout_with_id, "Check timer creation call", {})
        tests.assert_called(timer.cancel, "Check timer cancel call", {{"2gig_pin_codes_restrictions_timer"}})
    end
end

return fixture
