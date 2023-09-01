local tests = require "tests"
local logging = require "logging"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.string")

local slots_mapper
local creator = loadfile("HUB:zwave/scripts/utils/slots_mapper/create")
local function get_slots_mapper()
    return slots_mapper or creator("my_name", item_id, "dict_id")
end

math.randomseed(42)

local shuffle = loadfile("TEST:scripts/utils/shuffle_array")

for i = 1,111 do
    print("Values generation number: " .. i)

    if i == 100 then
        logging.trace("Slots mapper saved")
        slots_mapper = get_slots_mapper()
    end

    local values = {}

    local size = math.random(0, 20)

    while #values < size do
        table.insert(values, tostring(math.random(0, 10)))
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
