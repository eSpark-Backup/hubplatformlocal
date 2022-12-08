local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.weekly_interval")

local creator = loadfile("HUB:zwave/scripts/utils/slots_mapper/create")

local function get_slots_mapper()
    return creator("week_day_per_slot", item_id, "dict_id", 1, 2)
end

do
    get_slots_mapper():call("on_slot_value_changed", 1, {
            weekDay = "monday",
            startTime = "11:30:00",
            stopTime = "23:59:00",
        })

    get_slots_mapper():call("on_slot_value_changed", 2, {
            weekDay = "monday",
            startTime = "12:30:00",
            stopTime = "23:59:00",
        })

    get_slots_mapper():call("on_slot_value_changed", 3, {
            weekDay = "tuesday",
            startTime = "00:00:00",
            stopTime = "04:00:00",
        })

    get_slots_mapper():call("on_slot_value_changed", 4, {
            weekDay = "friday",
            startTime = "12:30:00",
            stopTime = "23:59:00",
        })


    tests.assert_eq(
        get_slots_mapper():call("get_values"), 
        {
            {
                weekDays = { "monday" },
                startTime = "11:30:00",
                stopTime = "04:00:00"
            },
            {
                weekDays = { "monday", "friday" },
                startTime = "12:30:00",
                stopTime = "23:59:00"
            }
        }, 
        "check merged values after group interruption with free morning part")

    get_slots_mapper():call("on_slot_value_changed", 4 )
    get_slots_mapper():call("on_slot_value_changed", 3 )
    get_slots_mapper():call("on_slot_value_changed", 2 )
    get_slots_mapper():call("on_slot_value_changed", 1 )
end

do
    get_slots_mapper():call("on_slot_value_changed", 1, {
            weekDay = "monday",
            startTime = "11:30:00",
            stopTime = "23:59:00",
        })

    get_slots_mapper():call("on_slot_value_changed", 2, {
            weekDay = "tuesday",
            startTime = "00:00:00",
            stopTime = "04:00:00",
        })

    get_slots_mapper():call("on_slot_value_changed", 3, {
            weekDay = "tuesday",
            startTime = "00:00:00",
            stopTime = "05:00:00",
        })

    get_slots_mapper():call("on_slot_value_changed", 4, {
            weekDay = "monday",
            startTime = "00:00:00",
            stopTime = "04:00:00",
        })

    tests.assert_eq(
        get_slots_mapper():call("get_values"), 
        {
            {
                weekDays = { "monday", "tuesday" },
                startTime = "00:00:00",
                stopTime = "04:00:00",
            },
            {
                weekDays = { "monday" },
                startTime = "11:30:00",
                stopTime = "05:00:00",
            },
        }, 
        "check merged values after group interruption with free evening part")

    get_slots_mapper():call("on_slot_value_changed", 4 )
    get_slots_mapper():call("on_slot_value_changed", 3 )
    get_slots_mapper():call("on_slot_value_changed", 2 )
    get_slots_mapper():call("on_slot_value_changed", 1 )
end
