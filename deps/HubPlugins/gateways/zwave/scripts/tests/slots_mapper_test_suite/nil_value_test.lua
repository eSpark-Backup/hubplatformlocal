local tests = require "tests"

local _, item_id = loadfile("HUB:zwave/scripts/tests/slots_mapper_test_suite/utils/create_fake_device_with_item")("dictionary.array.string")

local function get_slots_mapper()
    return loadfile("HUB:zwave/scripts/utils/slots_mapper/create")("my_name", item_id, "dict_id")
end

tests.assert_eq(#(get_slots_mapper():call("set_values")), 0, "check set nil values when prev values are ampty")

for _, command in ipairs(get_slots_mapper():call("set_values", { "1", "5" })) do
    get_slots_mapper():call("on_slot_value_changed", command.slot, command.value)
end

tests.assert_eq(#(get_slots_mapper():call("get_values")), 2, "check values size")

for _, command in ipairs(get_slots_mapper():call("set_values")) do
    get_slots_mapper():call("on_slot_value_changed", command.slot, command.value)
end

tests.assert_eq(get_slots_mapper():call("get_values"), {}, "check empty values after nil setting")
