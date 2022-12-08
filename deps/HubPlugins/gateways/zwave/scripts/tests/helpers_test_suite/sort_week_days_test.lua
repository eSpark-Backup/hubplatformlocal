local tests = require "tests"

local sort_week_days = loadfile("HUB:zwave/scripts/helpers/sort_week_days")
local week_days = { "friday", "wednesday", "monday", "saturday", "thursday", "sunday", "tuesday" }
local sorted_week_days = sort_week_days(week_days)

tests.assert_eq(sorted_week_days == week_days, true, "compare pointers of passed and returned week days tables")

tests.assert_eq(
    week_days, 
    { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" }, 
    "check full sorted week days array")

tests.assert_eq(
    sort_week_days( { "monday", "saturday", "sunday" } ), 
    { "sunday", "monday", "saturday" } , 
    "check sorted 3 week days")
