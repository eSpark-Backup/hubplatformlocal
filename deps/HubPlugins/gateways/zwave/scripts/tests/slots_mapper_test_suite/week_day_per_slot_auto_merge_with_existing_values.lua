local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.weekly_interval")

local creator = loadfile("HUB:zwave/scripts/utils/slots_mapper/create")

local function get_slots_mapper()
    return creator("week_day_per_slot", item_id, "dict_id", 1, 2)
end


get_slots_mapper():call("on_slot_value_changed", 1, {
        weekDay = "monday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })

get_slots_mapper():call("on_slot_value_changed", 2, {
        weekDay = "sunday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })

tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    {
        {
            weekDays = { "sunday", "monday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        }
    }, 
    "check two merged slot values")

tests.assert_eq(
    get_slots_mapper():call("set_values", get_slots_mapper():call("get_values")), 
    {}, 
    "check empty commands after setting the same values")

get_slots_mapper():call("on_slot_value_changed", 3, {
        weekDay = "saturday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })

tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    {
        {
            weekDays = { "sunday", "monday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        },
        {
            weekDays = { "saturday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        }
    }, 
    "check new value adding instead of merging to existing values")

get_slots_mapper():call("on_slot_value_changed", 4, {
        weekDay = "saturday",
        startTime = "11:30:00",
        stopTime = "12:31:00",
    })

tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    {
        {
            weekDays = { "sunday", "monday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        },
        {
            weekDays = { "saturday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        },
        {
            weekDays = { "saturday" },
            startTime = "11:30:00",
            stopTime = "12:31:00"
        }
    }, 
    "check new different value adding instead of merging")
