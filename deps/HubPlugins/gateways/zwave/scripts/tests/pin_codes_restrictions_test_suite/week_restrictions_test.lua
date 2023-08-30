local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local process = loadfile("HUB:zwave/scripts/helpers/virtual_pin_codes_restrictions/process_restrictions")
local core = require("core")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/virtual_pin_codes_restrictrion_fixture")()
fixture.init()

fixture.add_device()

local pin_codes_item_id = assert(fixture.find_item_id(item_names.user_codes, class_ids.user_codes))
local weekly_user_code_intervals_item_id = assert(fixture.find_item_id(item_names.weekly_user_code_intervals, class_ids.user_codes))

-- add pin code
core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1"})

-- test no restrictions
do
    process()

    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1"},
        }
    )

    fixture.check_event_table(nil)
    fixture.check_timer(false)
end

-- test sigle restriction disabled
do
    core.is_inside_week_interval = function(...) return false end
    local calls_count = 0
    local next_week_events = {"2022-01-05T11:51:54", "2021-01-05T11:51:54"}
    core.get_next_week_event = function(...)
        calls_count = calls_count + 1
        return next_week_events[calls_count]
    end

    core.update_item_dictionary_value(
        weekly_user_code_intervals_item_id,
        "1",
        {
            {
                weekDays = { "monday", "thursday" },
                startTime = "11:51:54",
                stopTime = "17:59:59"
            }
        }
    )

    fixture.set_local_time("2021-01-04T01:01:01")

    process()

    fixture.check_event_table({
        year = 2021, mounth = 1, day = 5,
        hour = 11, minute = 51, second = 54
    })
    fixture.check_timer(true, 82740000)

    fixture.check_pin_code_status_changed({{1, "disabled", "1111"}})
end

-- test sigle restriction disabled, no next event
do
    core.is_inside_week_interval = function(...) return false end
    core.get_next_week_event = function(...)
        return nil
    end

    core.update_item_dictionary_value(
        weekly_user_code_intervals_item_id,
        "1",
        {
            {
                weekDays = { "monday", "thursday" },
                startTime = "11:51:54",
                stopTime = "17:59:59"
            }
        }
    )

    fixture.set_local_time("2021-01-04T01:01:01")

    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "enabled"})

    process()

    fixture.check_event_table(nil)
    fixture.check_timer(false)

    fixture.check_pin_code_status_changed({{1, "disabled", "1111"}})
end

-- test sigle restriction enabled
do
    core.is_inside_week_interval = function(...) return true end
    local calls_count = 0
    local next_week_events = {"2022-01-05T11:51:54", "2021-01-05T11:51:54"}
    core.get_next_week_event = function(...)
        calls_count = calls_count + 1
        return next_week_events[calls_count]
    end

    core.update_item_dictionary_value(
        weekly_user_code_intervals_item_id,
        "1",
        {
            {
                weekDays = { "monday", "thursday" },
                startTime = "11:51:54",
                stopTime = "17:59:59"
            }
        }
    )

    fixture.set_local_time("2021-01-04T01:01:01")

    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "disabled"})

    process()

    fixture.check_event_table({
        year = 2021, mounth = 1, day = 5,
        hour = 11, minute = 51, second = 54
    })
    fixture.check_timer(true, 82740000)

    fixture.check_pin_code_status_changed({{1, "enabled", "1111"}})
end

-- test several restrictions
do
    local intervals_calls_count = 0
    local next_intervals = {false, true}
    core.is_inside_week_interval = function(...)
        intervals_calls_count = intervals_calls_count + 1
        return next_intervals[intervals_calls_count]
    end

    local events_calls_count = 0
    local next_week_events = {
        "2022-01-05T11:51:54",
        "2021-01-05T11:51:54",
        "2023-01-05T11:51:54",
        "2024-01-05T11:51:54"
    }
    core.get_next_week_event = function(...)
        events_calls_count = events_calls_count + 1
        return next_week_events[events_calls_count]
    end

    core.update_item_dictionary_value(
        weekly_user_code_intervals_item_id,
        "1",
        {
            {
                weekDays = { "monday", "thursday" },
                startTime = "11:51:54",
                stopTime = "17:59:59"
            },
            {
                weekDays = { "tuesday", "thursday" },
                startTime = "13:51:54",
                stopTime = "16:59:59"
            }
        }
    )

    fixture.set_local_time("2021-01-04T01:01:01")

    process()

    fixture.check_event_table({
        year = 2021, mounth = 1, day = 5,
        hour = 11, minute = 51, second = 54
    })
    fixture.check_timer(true, 82740000)

    fixture.check_pin_code_status_changed({{1, "enabled", "1111"}})
end

-- test several pin codes
do
    core.is_inside_week_interval = function(week_days, start_time, end_time)
        return week_days[1] == "wednesday"
    end

    local events_calls_count = 0
    local next_week_events = {
        "2022-01-05T11:51:54",
        "2021-01-05T11:51:54",
        "2023-01-05T11:51:54",
        "2024-01-05T11:51:54",
        "2024-01-05T11:51:54",
        "2024-01-05T11:51:54"
    }
    core.get_next_week_event = function(...)
        events_calls_count = events_calls_count + 1
        return next_week_events[events_calls_count]
    end

    core.update_item_dictionary_value(
        weekly_user_code_intervals_item_id,
        "1",
        {
            {
                weekDays = { "monday", "thursday" },
                startTime = "11:51:54",
                stopTime = "17:59:59"
            },
            {
                weekDays = { "tuesday", "thursday" },
                startTime = "13:51:54",
                stopTime = "16:59:59"
            }
        }
    )

    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1", mode = "enabled"})
    core.update_item_dictionary_value(pin_codes_item_id, "2", {code = "2222", name = "test2", mode = "disabled"})
    core.update_item_dictionary_value(
        weekly_user_code_intervals_item_id,
        "2",
        {
            {
                weekDays = { "wednesday", "thursday" },
                startTime = "11:51:54",
                stopTime = "17:59:59"
            }
        }
    )

    fixture.set_local_time("2021-01-04T01:01:01")

    process()

    fixture.check_event_table({
        year = 2021, mounth = 1, day = 5,
        hour = 11, minute = 51, second = 54
    })
    fixture.check_timer(true, 82740000)

    fixture.check_pin_code_status_changed({{1, "disabled", "1111"}, {2, "enabled", "2222"}})
end
