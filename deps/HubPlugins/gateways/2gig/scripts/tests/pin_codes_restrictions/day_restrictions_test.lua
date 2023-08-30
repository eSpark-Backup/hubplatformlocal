local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_names = require("HUB:2gig/scripts/enum/item_names")
local process = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")
local core = require("core")

local fixture = loadfile("HUB:2gig/scripts/tests/pin_codes_restrictions/fixture/pin_codes_restrictrion_fixture")()
fixture.init()

fixture.add_device("2GIG-PAD1-345", 123)

local pin_codes_item_id = test_utils.get_item_id(123, item_names.user_codes)
local daily_user_code_intervals_item_id = test_utils.get_item_id(123, item_names.daily_user_code_intervals)

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

-- test restriction in past
do
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-01-01T01:01:01",
                stopDateTime = "2021-02-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2022-01-01T01:01:01")

    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "disabled"},
        }
    )
    
    fixture.check_event_table(nil)    
    fixture.check_timer(false)
end

-- test restriction in future
do
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-02-01T06:05:07",
                stopDateTime = "2021-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2021-01-01T01:01:01")
    
    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "disabled"},
        }
    )
    
    fixture.check_event_table({
        year = 2021, mounth = 2, day = 1,
        hour = 6, minute = 5, second = 7
    })
    
    fixture.check_timer(true, 82740000)
end

-- test restriction enabled
do
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-02-01T06:05:07",
                stopDateTime = "2021-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2021-02-05T01:01:01")

    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "enabled"},
        }
    )
    
    fixture.check_event_table({
        year = 2021, mounth = 3, day = 30,
        hour = 23, minute = 59, second = 59
    })
    
    fixture.check_timer(true, 82740000)
end

-- test two restrictions disabled
do
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-02-01T06:05:07",
                stopDateTime = "2021-03-30T23:59:59"
            },
            {
                startDateTime = "2023-02-01T06:05:07",
                stopDateTime = "2023-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2022-02-05T01:01:01")

    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "disabled"},
        }
    )
    
    fixture.check_event_table({
        year = 2023, mounth = 2, day = 1,
        hour = 6, minute = 5, second = 7
    })
    
    fixture.check_timer(true, 82740000)
end

-- test two restrictions one enabled
do
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-02-01T06:05:07",
                stopDateTime = "2021-03-30T23:59:59"
            },
            {
                startDateTime = "2023-02-01T06:05:07",
                stopDateTime = "2023-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2021-02-05T01:01:01")

    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "enabled"},
        }
    )
    
    fixture.check_event_table({
        year = 2021, mounth = 3, day = 30,
        hour = 23, minute = 59, second = 59
    })
    
    fixture.check_timer(true, 82740000)
end

-- test two pin codes one enabled
do
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-02-01T06:05:07",
                stopDateTime = "2021-03-30T23:59:59"
            },
            {
                startDateTime = "2023-02-01T06:05:07",
                stopDateTime = "2023-03-30T23:59:59"
            }
        }
    )
    
    core.update_item_dictionary_value(pin_codes_item_id, "2", {code = "2222", name = "test2"})
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "2", 
        {
            {
                startDateTime = "2021-03-01T06:05:07",
                stopDateTime = "2022-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2021-02-05T01:01:01")

    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "enabled"},
            ["2"] = {code = "2222", name = "test2", mode = "disabled"},
        }
    )
    
    fixture.check_event_table({
        year = 2021, mounth = 3, day = 1,
        hour = 6, minute = 5, second = 7
    })
    
    fixture.check_timer(true, 82740000)
end

-- test two devices
do
    fixture.add_device("2GIG-PAD1-345", 124)
    
    local device2_pin_codes_item_id = test_utils.get_item_id(124, item_names.user_codes)
    local device2_daily_user_code_intervals_item_id = test_utils.get_item_id(124, item_names.daily_user_code_intervals)

    core.update_item_dictionary_value(device2_pin_codes_item_id, "5", {code = "5555", name = "test5"})


    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-03-01T06:05:07",
                stopDateTime = "2021-04-30T23:59:59"
            },
            {
                startDateTime = "2023-02-01T06:05:07",
                stopDateTime = "2023-03-30T23:59:59"
            }
        }
    )
    
    core.update_item_dictionary_value(
        device2_daily_user_code_intervals_item_id, 
        "5", 
        {
            {
                startDateTime = "2021-02-01T06:05:07",
                stopDateTime = "2022-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2021-02-05T01:01:01")

    process()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "disabled"},
            ["2"] = {code = "2222", name = "test2", mode = "disabled"},
        }
    )
    
    fixture.check_item_value(
        device2_pin_codes_item_id,
        {
            ["5"] = {code = "5555", name = "test5"},
        }
    )
    
    fixture.check_event_table({
        year = 2021, mounth = 3, day = 1,
        hour = 6, minute = 5, second = 7
    })
    
    fixture.check_timer(true, 82740000)
end
