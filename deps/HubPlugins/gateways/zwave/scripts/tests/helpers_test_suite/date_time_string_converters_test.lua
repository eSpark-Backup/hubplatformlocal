local tests = require "tests"

local date_time_to_string = loadfile("HUB:zwave/scripts/helpers/date_time_to_string")
local time_to_string = loadfile("HUB:zwave/scripts/helpers/time_to_string")

local date_time_from_string = loadfile("HUB:zwave/scripts/helpers/date_time_from_string")
local time_from_string = loadfile("HUB:zwave/scripts/helpers/time_from_string")

tests.assert_eq(date_time_to_string(1999, 12, 30, 23, 59, 59), "1999-12-30T23:59:59", "check string date time")
tests.assert_eq({ date_time_from_string("1999-12-30T23:59:59") }, { 1999, 12, 30, 23, 59, 59 }, "check date time")

tests.assert_eq(time_to_string(11, 2, 4), "11:02:04", "check string time")
tests.assert_eq({ time_from_string("12:08:00") }, { 12, 8, 0 }, "check time")

tests.assert_failed("Check date time invalid format with space", date_time_from_string, "1999-12-30 23:59:00")
tests.assert_failed("Check date time invalid format with ':' separator", date_time_from_string, "1999:12:30T23:59:00")

tests.assert_failed("Check time invalid format with '-' separator", time_from_string, "23-59-00")