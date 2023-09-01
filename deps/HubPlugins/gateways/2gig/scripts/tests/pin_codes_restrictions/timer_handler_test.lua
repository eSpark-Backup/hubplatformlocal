local test_utils = require("HUB:2gig/scripts/tests/test_utils")
local item_names = require("HUB:2gig/scripts/enum/item_names")
local handler = loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/timer/handler")
local core = require("core")

local fixture = loadfile("HUB:2gig/scripts/tests/pin_codes_restrictions/fixture/pin_codes_restrictrion_fixture")()
fixture.init()

fixture.add_device("2GIG-PAD1-345", 123)

local pin_codes_item_id = test_utils.get_item_id(123, item_names.user_codes)
local daily_user_code_intervals_item_id = test_utils.get_item_id(123, item_names.daily_user_code_intervals)

-- init
do
    core.update_item_dictionary_value(pin_codes_item_id, "1", {code = "1111", name = "test1"})
    
    core.update_item_dictionary_value(
        daily_user_code_intervals_item_id, 
        "1", 
        {
            {
                startDateTime = "2021-02-01T01:01:01",
                stopDateTime = "2021-03-30T23:59:59"
            }
        }
    )
    
    fixture.set_local_time("2020-01-01T01:01:01")
    
    loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
end

-- test time not reached
do
    fixture.set_local_time("2021-01-01T01:01:01")
    
    handler()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "disabled"},
        }
    )
    
    fixture.check_timer(true, 82740000)
end

-- test time enable reached
do
    fixture.set_local_time("2021-02-01T01:01:02")
    
    handler()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "enabled"},
        }
    )
    
    fixture.check_timer(true, 82739000)
end

-- test time disable not reached
do
    fixture.set_local_time("2021-02-05T01:01:01")
    
    handler()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "enabled"},
        }
    )
end

-- test time disable reached
do
    fixture.set_local_time("2021-04-30T23:59:59")
    
    handler()
    
    fixture.check_item_value(
        pin_codes_item_id,
        {
            ["1"] = {code = "1111", name = "test1", mode = "disabled"},
        }
    )
    fixture.check_event_table(nil)
    fixture.check_timer(false)    
end
