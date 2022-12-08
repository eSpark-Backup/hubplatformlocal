local tests = require("tests")
local core = require("core")
local get_duration_to_next_event = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/get_duration_to_next_event")

core.get_local_time = function(...)
    return "2020-02-03T11:50:54+0200"
end

-- test next day
tests.assert_eq( 
    get_duration_to_next_event(
        {
            year = 2020, mounth = 2, day = 4,
            hour = 14, minute = 25, second = 10
        }
    ), 
    43747000
)

-- test same day
tests.assert_eq( 
    get_duration_to_next_event(
        {
            year = 2020, mounth = 2, day = 3,
            hour = 13, minute = 35, second = 18
        }
    ), 
    6264000
)
