local test = require( "tests" )

local deep_equal = loadfile("HUB:zwave/scripts/helpers/deep_equal")

test.assert_eq(deep_equal(nil, nil), true, "check nil equal")
test.assert_eq(deep_equal(nil, {}), false, "check nil and empty table not equal")
test.assert_eq(deep_equal({}, nil), false, "2nd check nil and empty table not equal")
test.assert_eq(deep_equal(10, 10), true, "check equal integers")
test.assert_eq(deep_equal("s", "e"), false, "check not equal strings")

local some_function = function() end
local some_table = { 
    1,
    10,
    x = "s", 
    { 
        f = some_function,
        y = "s"
    } 
}

local equal_table = { 
    1,
    10,
    x = "s", 
    { 
        f = some_function,
        y = "s"
    } 
}

test.assert_eq(deep_equal(some_table, equal_table), true, "check table equal")
test.assert_eq(deep_equal(equal_table, some_table), true, "2nd check table equal" )
test.assert_eq(deep_equal(some_table, some_table), true, "check the same tables")


local similar_table_1 = { 
    1,
    10,
    x = "s", 
    { 
        f = function() end,
        y = "s"
    } 
}

local similar_table_2 = { 
    1,
    10,
    x = "s", 
    { 
        g = some_function,
        y = "s"
    } 
}

test.assert_eq(deep_equal(some_table, similar_table_1), false, "check similar table 1 not equal")
test.assert_eq(deep_equal(similar_table_1, some_table), false, "2nd check similar table 1 not equal")

test.assert_eq(deep_equal(some_table, similar_table_2), false, "check similar table 2 not equal")
test.assert_eq(deep_equal(similar_table_2, some_table), false, "2nd check similar table 2 not equal")

test.assert_eq(deep_equal({ [{}] = 1 }, { [{}] = 1 }), false, "check tables with deeply equal but not the same table keys")
