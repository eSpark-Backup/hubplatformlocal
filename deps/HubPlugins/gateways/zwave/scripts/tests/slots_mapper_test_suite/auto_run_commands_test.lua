local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.string")

local function get_slots_mapper()
    return hubplugins.object("slots_mapper_with_command_run", 
        "HUB:zwave/scripts/tests/slots_mapper_test_suite",
        "test_name",
        item_id, 
        "dict_id")
end

local function run_commands()
    local steps_count = 0
    while get_slots_mapper():call("do_command_run_step") do  
        steps_count = steps_count + 1
    end
    return steps_count 
end

get_slots_mapper():call("set_values", { 1, 5, 10, { "x" } }, true)
tests.assert_eq(run_commands(), 4, "check steps count after adding 4 values")
tests.assert_eq(get_slots_mapper():call("get_values"), { 1, 5, 10, { "x" } }, "check values after adding 4 values")

get_slots_mapper():call("set_values", { 10, 5, { "x" }, 1 }, true)
tests.assert_eq(run_commands(), 0, "check steps count after setting equal values in different order")
tests.assert_eq(get_slots_mapper():call("get_values"), { 10, 5, { "x" }, 1 }, "check values after setting equal values in different order")

get_slots_mapper():call("set_values", { 10, "6" }, true)
tests.assert_eq(run_commands(), 3, "check steps count after removing and replacement of several values")
tests.assert_eq(get_slots_mapper():call("get_values"), { 10, "6" }, "check values after removing and replacement of several values")

get_slots_mapper():call("set_values", { }, true)
tests.assert_eq(run_commands(), 2, "check steps count after all values removing")
tests.assert_eq(get_slots_mapper():call("get_values"), { }, "check values after all values removing")
