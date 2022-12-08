local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.weekly_interval")

math.randomseed(9000)

local creator = loadfile("HUB:zwave/scripts/utils/slots_mapper/create")

local function get_slots_mapper()
    return creator("week_day_per_slot", item_id, "dict_id", 1, 2)
end

local shuffle = loadfile("TEST:scripts/utils/shuffle_array")
local time_to_string = loadfile("HUB:zwave/scripts/helpers/time_to_string")

for i = 1, 20 do
    print("shifted week days auto generation number " .. i)

    local start_time = time_to_string(22, i, 0)
    local stop_time = time_to_string(21, i, 0)

    local value_1 = {
        weekDays = { "sunday", "saturday", "wednesday" },
        startTime = start_time,
        stopTime = stop_time
    }

    local value_2 = {
        weekDays = { "sunday", "friday", "wednesday" },
        startTime = start_time,
        stopTime = "23:59:00"
    }

    local value_3 = {
        weekDays = { "sunday", "monday", "saturday" },
        startTime = "00:00:00",
        stopTime = stop_time
    }

    local value_4 = {
        weekDays = { "saturday" },
        startTime = start_time,
        stopTime = stop_time
    }

    local value_5 = {
        weekDays = { "sunday", "monday" },
        startTime = "00:00:00",
        stopTime = "21:02:00"
    }

    local values = { value_1, value_2, value_3, value_4, value_5 }
    shuffle(values)

    local commands = get_slots_mapper():call("set_values", values)

    tests.assert_eq(
        #commands, 
        (#value_1.weekDays + #value_4.weekDays) * 2 + 
        #value_2.weekDays + 
        #value_3.weekDays + 
        (i == 1 and #value_5.weekDays or 0), 
        "check commands count" )

    shuffle(commands)

    for _, command in ipairs(commands) do
        get_slots_mapper():call("on_slot_value_changed", command.slot, command.value)
    end

    tests.assert_eq(get_slots_mapper():call("get_values"), values, "check values (shifted week day)")
end
