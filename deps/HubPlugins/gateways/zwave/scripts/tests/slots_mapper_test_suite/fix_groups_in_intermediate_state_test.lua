local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.string")

local function get_slots_mapper()
    return hubplugins.object("slots_mapper_with_values_merge", 
        "HUB:zwave/scripts/tests/slots_mapper_test_suite", 
        item_id, 
        "dict_id")
end


local commands = get_slots_mapper():call("set_values", { { "a", "b", "c" }, { "d", "e", "f" } })

for _, command in ipairs(commands) do
    get_slots_mapper():call("on_slot_value_changed", command.slot, command.value)
end


commands = get_slots_mapper():call("set_values", { { "a" }, { "e", "f" } })

tests.assert_eq(#commands, 3, "check commands size")

get_slots_mapper():call("on_slot_value_changed", commands[1].slot, commands[1].value)
tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    { { "a", "c" }, { "d", "e", "f" } }, 
    "check values after first subvalue removing")

get_slots_mapper():call("on_slot_value_changed", commands[1].slot, commands[1].value)
tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    { { "a", "c" }, { "d", "e", "f" } }, 
    "check values after 1st subvalue removing")

get_slots_mapper():call("on_slot_value_changed", commands[3].slot, commands[3].value)
tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    { { "a", "c" }, { "e", "f" } }, 
    "check values after 2nd subvalue removing")

get_slots_mapper():call("on_slot_value_changed", commands[2].slot, commands[2].value)
tests.assert_eq(
    get_slots_mapper():call("get_values"), 
    { { "a" }, { "e", "f" } }, 
    "check values after 3rd (last) subvalue removing")
