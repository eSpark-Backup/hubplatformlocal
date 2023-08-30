
local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.weekly_interval")

math.randomseed(100500)

local creator = loadfile("HUB:zwave/scripts/utils/slots_mapper/create")

local function get_slots_mapper()
    return creator("week_day_per_slot", item_id, "dict_id", 1, 2)
end

local shuffle = loadfile("TEST:scripts/utils/shuffle_array")

local function weekdays_comparator(a, b)
    if #a ~= #b then
        return #a < #b
    end

    for i, weekDayA in ipairs(a) do
        local weekDayB = b[i]
        if weekDayA < weekDayB then
            return true
        elseif weekDayB < weekDayA then
            return false
        end
    end

    return false
end

local function value_comparator(a, b)
    return a.startTime < b.startTime or
        a.startTime == b.startTime and a.stopTime < b.stopTime or
        a.stopTime == b.stopTime and weekdays_comparator(a.weekDays, b.weekDays)
end

local expected_values = {
    {
        weekDays = { "sunday", "monday", "saturday" },
        startTime = "22:30:00",
        stopTime = "09:30:00"
    },
    {
        weekDays = { "monday" },
        startTime = "00:00:00",
        stopTime = "23:59:00"
    },
    {
        weekDays = { "monday", "friday" },
        startTime = "00:00:00",
        stopTime = "07:31:00"
    },
    {
        weekDays = { "monday" },
        startTime = "19:31:00",
        stopTime = "23:59:00"
    }
}

table.sort( expected_values, value_comparator )

local function get_actual_values()
    local actual_values = {}
    for i, value in ipairs(get_slots_mapper():call("get_values")) do
        actual_values[i] = value
    end
    table.sort( actual_values, value_comparator )
    return actual_values
end


for i = 1, 77 do
    local sub_values = {
        {
            weekDay = "monday",
            startTime = "22:30:00",
            stopTime = "23:59:00",
        },
        {
            weekDay = "tuesday",
            startTime = "00:00:00",
            stopTime = "09:30:00",
        },
        {
            weekDay = "saturday",
            startTime = "22:30:00",
            stopTime = "23:59:00",
        },
        {
            weekDay = "sunday",
            startTime = "00:00:00",
            stopTime = "09:30:00",
        },
        {
            weekDay = "sunday",
            startTime = "22:30:00",
            stopTime = "23:59:00",
        },
        {
            weekDay = "monday",
            startTime = "00:00:00",
            stopTime = "09:30:00",
        },
        {
            weekDay = "monday",
            startTime = "00:00:00",
            stopTime = "23:59:00",
        },
        {
            weekDay = "monday",
            startTime = "19:31:00",
            stopTime = "23:59:00",
        },
        {
            weekDay = "monday",
            startTime = "00:00:00",
            stopTime = "07:31:00",
        },
        {
            weekDay = "friday",
            startTime = "00:00:00",
            stopTime = "07:31:00",
        }
    }

    shuffle(sub_values)

    for slot, sub_value in ipairs(sub_values) do
        get_slots_mapper():call("on_slot_value_changed", slot, sub_value)
    end

    local actual_values = get_actual_values()

    tests.assert_eq(#actual_values, #expected_values, "check size of merged shifted days")
    tests.assert_eq(actual_values, expected_values, "check merged shifted days")

    for slot, sub_value in ipairs(sub_values) do
        get_slots_mapper():call("on_slot_value_changed", slot, sub_value)
        tests.assert_eq(get_actual_values(), expected_values, "check value not changed if slot repeated the same report")
    end

    for i in ipairs(sub_values) do
        get_slots_mapper():call("on_slot_value_changed", i, nil)
    end

    tests.assert_eq(get_slots_mapper():call("get_values"), {}, "check removed values")
end

