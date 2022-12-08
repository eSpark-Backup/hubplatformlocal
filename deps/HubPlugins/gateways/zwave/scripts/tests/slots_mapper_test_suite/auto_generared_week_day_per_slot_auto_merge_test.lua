local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.weekly_interval")

local creator = loadfile("HUB:zwave/scripts/utils/slots_mapper/create")

local function get_slots_mapper()
    return creator("week_day_per_slot", item_id, "dict_id", 1, 2, true)
end

math.randomseed(314)

local shuffle = loadfile("TEST:scripts/utils/shuffle_array")
local sort_week_days = loadfile("HUB:zwave/scripts/helpers/sort_week_days")
local time_to_string = loadfile("HUB:zwave/scripts/helpers/time_to_string")

local function generate_week_days()
    local week_days = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" }
    local count = math.random(2, 8)
    for i = 7, count, -1 do
        local index = math.random(1, i)
        week_days[index] = week_days[i]
        week_days[i] = nil
    end

    return sort_week_days(week_days)
end


local function generate_time(times_set)
    local result
    repeat
        result = time_to_string(math.random(0, 23), math.random(0, 59), 0)
    until times_set[result] == nil
    times_set[result] = true
    return result 
end

for i = 1,55 do
    print("Weekly interval values generation number: " .. i)
    local values = {}
    local sub_values = {}
    local slots = {}

    local values_count = math.random(1, 4)

    local times_set = {
        ["00:00:00"] = true,
        ["23:59:00"] = true
    }

    for j = 1, values_count do
        local value = {
            weekDays = generate_week_days(),
            startTime = generate_time(times_set),
            stopTime = generate_time(times_set)
        }

        table.insert(values, value)

        for _, week_day in ipairs(value.weekDays) do
            table.insert(sub_values, {
                    weekDay = week_day,
                    startTime = value.startTime,
                    stopTime = value.stopTime
                })

            table.insert(slots, #slots + 1)
        end
    end

    shuffle(sub_values)
    shuffle(slots)

    for j, sub_value in ipairs(sub_values) do
        get_slots_mapper():call("on_slot_value_changed", slots[j], sub_value)
    end

    local function values_comparator(a, b)
        return a.startTime < b.startTime or a.startTime == b.startTime and a.stopTime < b.stopTime
    end

    local actual_values = get_slots_mapper():call("get_values")

    table.sort(values, values_comparator)
    table.sort(actual_values, values_comparator)

    tests.assert_eq(#actual_values, #values, "check auto merged values size")
    tests.assert_eq(actual_values, values, "check auto merged values")

    for _, slot in ipairs(shuffle(slots)) do
        get_slots_mapper():call("on_slot_value_changed", slot, nil)
    end

    tests.assert_eq(#get_slots_mapper():call("get_values"), 0, "check cleared values")
end
