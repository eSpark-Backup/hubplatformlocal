local less_or_equal = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/less_or_equal")
local time_is_in_interval = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/time_is_in_interval")
local tests = require("tests")
local core = require("core")

-- test equal
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test seconds less
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 53
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test minutes less
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 49, second = 59
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test hours less
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 10, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test day less
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 2,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test mounth less
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 5,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test year less
tests.assert_eq(
    less_or_equal(
        {
            year = 2019, mounth = 5, day = 6,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test seconds more
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 55
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test minutes more
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 51, second = 53
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test hours more
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 3,
            hour = 12, minute = 49, second = 53
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test days more
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 1, day = 4,
            hour = 10, minute = 49, second = 53
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test mounth more
tests.assert_eq(
    less_or_equal(
        {
            year = 2020, mounth = 2, day = 1,
            hour = 10, minute = 49, second = 53
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test year more
tests.assert_eq(
    less_or_equal(
        {
            year = 2021, mounth = 1, day = 1,
            hour = 10, minute = 49, second = 53
        },
        {
            year = 2020, mounth = 1, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test interval lower bound
core.get_local_time = function(...)
    return "2019-05-06T12:51:59+0200"
end

tests.assert_eq(
    time_is_in_interval(
        {
            year = 2019, mounth = 5, day = 6,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test interval upper bound
core.get_local_time = function(...)
    return "2020-02-03T11:50:54+0200"
end

tests.assert_eq(
    time_is_in_interval(
        {
            year = 2019, mounth = 5, day = 6,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test interval inside
core.get_local_time = function(...)
    return "2019-06-03T11:50:54+0200"
end

tests.assert_eq(
    time_is_in_interval(
        {
            year = 2019, mounth = 5, day = 6,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    true
)

-- test interval less then lower bound
core.get_local_time = function(...)
    return "2018-06-03T11:50:54+0200"
end

tests.assert_eq(
    time_is_in_interval(
        {
            year = 2019, mounth = 5, day = 6,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)

-- test interval higher then upper bound
core.get_local_time = function(...)
    return "2021-06-03T11:50:54+0200"
end

tests.assert_eq(
    time_is_in_interval(
        {
            year = 2019, mounth = 5, day = 6,
            hour = 12, minute = 51, second = 59
        },
        {
            year = 2020, mounth = 2, day = 3,
            hour = 11, minute = 50, second = 54
        }
    ), 
    false
)
