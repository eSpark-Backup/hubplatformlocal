local time_parser = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/parse_time_string")
local tests = require("tests")

local function check_parced(string, expect)
    local result = time_parser(string)
    tests.assert_eq(result, expect)
end

check_parced(
    "2020-01-03T11:50:54",
    {
        year = 2020, mounth = 1, day = 3,
        hour = 11, minute = 50, second = 54
    }
)

check_parced(
    "2004-05-23T14:25:10+0200",
    {
        year = 2004, mounth = 5, day = 23,
        hour = 14, minute = 25, second = 10
    }
)
