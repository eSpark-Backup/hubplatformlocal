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
        weekDay = "monday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })

get_slots_mapper():call("on_slot_value_changed", 3, {
        weekDay = "wednesday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })

tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    {
        {
            weekDays = { "monday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        },
        {
            weekDays = { "monday", "wednesday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        }
    }, 
    "check intermediate merge result of duplicated values")

get_slots_mapper():call("on_slot_value_changed", 4, {
        weekDay = "saturday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })

get_slots_mapper():call("on_slot_value_changed", 5, {
        weekDay = "wednesday",
        startTime = "11:30:00",
        stopTime = "12:30:00",
    })


tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    {
        {
            weekDays = { "monday", "wednesday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        },
        {
            weekDays = { "monday", "wednesday", "saturday" },
            startTime = "11:30:00",
            stopTime = "12:30:00"
        }
    }, 
    "check merge result of duplicated values")
