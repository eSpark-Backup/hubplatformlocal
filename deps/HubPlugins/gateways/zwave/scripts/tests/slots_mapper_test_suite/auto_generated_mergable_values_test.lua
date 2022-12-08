local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.array.array.string")

local function get_slots_mapper()
    return hubplugins.object("slots_mapper_with_values_merge", 
        "HUB:zwave/scripts/tests/slots_mapper_test_suite", 
        item_id, 
        "dict_id")
end

math.randomseed(142)

local shuffle = loadfile("TEST:scripts/utils/shuffle_array")

for i = 1,99 do
    print("Mergable values generation number: " .. i)

    local values = {}
    local size = math.random(0, 10)

    while #values < size do
        local value = {}
        local value_size = math.random(1, 4)
        while #value < value_size do
            table.insert(value, { tostring(math.random(0, 10)) })
        end

        table.insert(values, value)
    end

    local commands = get_slots_mapper():call("set_values", values)
    shuffle(commands)

    if #commands > 0 then
        tests.assert_ne(get_slots_mapper():call("get_values"), values, "intermediate values shouldn't be equal expected")
    end

    for _, command in ipairs(commands) do
        get_slots_mapper():call("on_slot_value_changed", command.slot, command.value)
    end

    tests.assert_eq(get_slots_mapper():call("get_values"), values, "check result of all commands execution")
end
